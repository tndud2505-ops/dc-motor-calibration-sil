# `code/sil_environment.c` 구현 상세 가이드

저장소의 학생 파일에는 함수 원형과 `TODO`만 들어 있습니다. 아래 코드 조각은 각 TODO를 어떤 순서와 원리로 구현할지 설명하는 학습 예시입니다.

## 1. 이 과제에서 SIL을 사용하는 이유

실제 모터, 전류 센서, Hall 센서, Stopper를 모든 수강생이 동일한 조건으로 준비하고 검증하기는 어렵습니다. 모터마다 마찰과 전류가 다르고, Stopper 위치와 센서 노이즈도 다르기 때문입니다.

이번 과제의 우선 목표는 실제 모터의 정밀한 물리 모델을 만드는 것이 아닙니다.

- 실제 제어 코드와 가상 환경을 함수로 분리하는 방법
- 주기 함수가 모터 상태를 갱신하는 방법
- 센서 이벤트가 인터럽트 함수로 전달되는 방법
- 로컬 C 빌드와 GitHub Actions를 사용하는 방법

위 네 가지 개념을 익히는 것이 목표입니다.

## 2. 파일 사이의 역할

```text
code/provided_control.c
    CAL, GET ON, GET OFF, STOP 제어 순서를 결정
            │ SIL_MotorCW(), SIL_MotorCCW(), SIL_MotorStop()
            ▼
code/sil_environment.c
    가상 모터 위치와 전류를 갱신
            │ EXTI6_IRQHandler(), ADC1_2_IRQHandler()
            ▼
code/provided_control.c
    Hall pulse와 ADC 전류값을 받아 제어 상태를 갱신
```

두 파일은 전역변수를 직접 공유하지 않습니다.

```c
/* code/provided_control.c 안에서만 사용 */
static uint32_t control_current_raw;

/* code/sil_environment.c 안에서만 사용 */
static uint32_t sil_current_raw;
```

이름도 다르고 각각 `static`이므로 서로 다른 메모리입니다. ADC 인터럽트가 실행될 때 `SIL_ReadCurrentRaw()`의 반환값만 제어 코드로 복사됩니다.

## 3. 가상 환경에서 보관할 상태

`code/sil_environment.c`에는 실제 장비를 대신하는 상태가 필요합니다.

```c
static int32_t physical_position;
static uint32_t motor_direction;
static uint32_t sil_current_raw;
```

각 변수의 의미는 다음과 같습니다.

| 변수 | 의미 |
| --- | --- |
| `physical_position` | 가상 모터의 실제 기계 위치. 0부터 100까지 이동 |
| `motor_direction` | 현재 정지, CW 또는 CCW 상태 |
| `sil_current_raw` | 가상 ADC가 읽을 전류 원시값 |

이 변수들은 제어 코드의 `current_position`과 다릅니다.

- `physical_position`: SIL 환경이 보관하는 가상 모터의 물리 위치
- `current_position`: 제어 코드가 Hall pulse를 계산해 추정한 위치

두 값이 같은 방향으로 움직이는지를 비교하는 것이 SIL 검증의 핵심입니다.

## 4. `SIL_Init()` 구현

프로그램을 처음 시작할 때 가상 장비 상태를 초기화합니다.

```c
void SIL_Init(void)
{
    physical_position = 50;
    motor_direction = DIRECTION_STOP;
    sil_current_raw = 0u;
}
```

- 실제 양 끝은 0과 100입니다.
- 시작 위치는 중간인 50으로 둡니다.
- 아직 모터 명령이 없으므로 방향은 STOP입니다.
- 정지 상태이므로 전류는 0입니다.

## 5. 모터 명령 함수 구현

실제 MCU에서는 GPIO와 PWM 레지스터를 설정하지만 SIL에서는 방향 상태만 저장합니다.

```c
void SIL_MotorCW(void)
{
    motor_direction = DIRECTION_CW;
}

void SIL_MotorCCW(void)
{
    motor_direction = DIRECTION_CCW;
}

void SIL_MotorStop(void)
{
    motor_direction = DIRECTION_STOP;
}
```

여기서는 위치를 즉시 변경하지 않습니다. 실제 모터도 명령을 받은 순간 목표 위치로 이동하지 않기 때문입니다. 위치 변화는 주기적으로 실행되는 `SIL_Tick()`에서 한 단계씩 처리합니다.

## 6. `SIL_Tick()`의 의미

`SIL_Tick()`은 실제 MCU에서 주기적으로 실행되는 제어 주기 한 번을 모사합니다.

예를 들어 실제 MCU의 main loop 또는 1 ms 주기 태스크가 한 번 실행됐다고 생각하면 됩니다. 이 예제에서 실제 시간 단위는 중요하지 않습니다. 한 번 호출될 때 가상 모터가 위치 1만큼 이동한다고 단순화합니다.

한 tick에서 다음 순서로 처리합니다.

```text
1. 현재 모터 방향 확인
2. 끝단이 아니라면 물리 위치를 1만큼 이동
3. 실제로 이동했다면 Hall 인터럽트 1회 발생
4. 정지/정상 이동/끝단 상태에 따라 전류값 결정
5. ADC 인터럽트 1회 발생
```

### 6-1. CW 이동과 Hall 인터럽트

```c
if (motor_direction == DIRECTION_CW &&
    physical_position < PHYSICAL_UPPER_POSITION)
{
    physical_position++;
    EXTI6_IRQHandler();
}
```

- CW 상태이고 UPPER `100`보다 작을 때만 이동합니다.
- 위치가 실제로 `+1` 움직였으므로 Hall pulse가 하나 발생합니다.
- Hall pulse를 모사하기 위해 `EXTI6_IRQHandler()`를 한 번 호출합니다.
- 이미 위치가 100이면 더 움직이지 않으며 Hall 인터럽트도 호출하지 않습니다.

### 6-2. CCW 이동과 Hall 인터럽트

```c
else if (motor_direction == DIRECTION_CCW &&
         physical_position > PHYSICAL_LOWER_POSITION)
{
    physical_position--;
    EXTI6_IRQHandler();
}
```

- CCW 상태이고 LOWER `0`보다 클 때만 이동합니다.
- 위치가 실제로 `-1` 움직였으므로 Hall pulse가 하나 발생합니다.
- 이미 위치가 0이면 더 움직이지 않으며 Hall 인터럽트도 호출하지 않습니다.

중요한 원칙은 `이동했을 때만 Hall 인터럽트를 발생시킨다`입니다. 끝단에서 위치는 멈췄는데 Hall 인터럽트를 계속 호출하면 제어 코드의 계산 위치만 계속 증가하거나 감소하게 됩니다.

### 6-3. 전류값 결정

```c
if (motor_direction == DIRECTION_STOP)
{
    sil_current_raw = 0u;
}
else if (physical_position == PHYSICAL_LOWER_POSITION ||
         physical_position == PHYSICAL_UPPER_POSITION)
{
    sil_current_raw = 3200u;
}
else
{
    sil_current_raw = 1500u;
}
```

전류는 세 가지 상태만 단순하게 모사합니다.

| 상태 | 가상 전류값 | 의미 |
| --- | ---: | --- |
| 모터 정지 | `0` | PWM 출력이 꺼진 상태 |
| 정상 이동 | `1500` | Stopper에 닿지 않고 회전 중 |
| 끝단에서 계속 구동 | `3200` | 모터가 Stall되어 전류가 증가한 상태 |

제어 코드의 Stall 판단 기준은 `2800`입니다. 따라서 정상 전류 `1500`은 기준보다 낮고, 끝단 전류 `3200`은 기준보다 높습니다.

끝단에서는 다음 두 현상이 동시에 발생합니다.

```text
물리 위치 변화 없음 → Hall 인터럽트 중지
모터 명령은 계속 유지 → 전류를 3200으로 상승
```

이 조합으로 제어 코드는 모터가 Stopper에 닿았다고 판단합니다.

### 6-4. ADC 인터럽트 발생

```c
ADC1_2_IRQHandler();
```

전류값을 결정한 뒤 매 tick마다 ADC 인터럽트를 한 번 호출합니다. ADC 인터럽트는 다음 순서로 값을 전달합니다.

```text
sil_current_raw
→ SIL_ReadCurrentRaw()
→ ADC1_2_IRQHandler()
→ control_current_raw
```

Hall 인터럽트는 실제 이동이 있을 때만 호출하지만, ADC 인터럽트는 정지 상태를 포함해 매 tick 호출합니다. 그래야 모터를 정지했을 때 전류가 `3200`에서 `0`으로 낮아진 사실도 제어 코드가 알 수 있습니다.

## 7. 센서값 반환 함수

```c
uint32_t SIL_ReadCurrentRaw(void)
{
    return sil_current_raw;
}

int32_t SIL_GetPhysicalPosition(void)
{
    return physical_position;
}
```

- `SIL_ReadCurrentRaw()`는 ADC 인터럽트가 읽을 가상 전류를 반환합니다.
- `SIL_GetPhysicalPosition()`은 `code/main.c`가 출력용으로 가상 물리 위치를 확인할 때 사용합니다.
- 반환 함수를 사용하며 다른 C 파일에서 SIL의 `static` 변수에 직접 접근하지 않습니다.

## 8. CALIBRATION이 진행되는 전체 순서

```text
초기 물리 위치 50
→ CAL 명령
→ SIL_MotorCW()
→ SIL_Tick()마다 위치 51, 52, ... 100
→ 이동할 때마다 EXTI6_IRQHandler() 호출
→ 100에 도달하면 Hall 인터럽트 중지, 전류 3200
→ ADC1_2_IRQHandler()가 높은 전류 전달
→ 제어 코드가 UPPER 도달로 판단하고 모터 정지
→ 다음 tick에서 전류 0
→ 제어 코드가 SIL_MotorCCW() 호출
→ SIL_Tick()마다 위치 99, 98, ... 0
→ 이동할 때마다 EXTI6_IRQHandler() 호출
→ 제어 코드가 Hall pulse 100개를 stroke로 계산
→ 0에 도달하면 Hall 인터럽트 중지, 전류 3200
→ 제어 코드가 LOWER 도달로 판단
→ LOWER=0, UPPER=100 저장
→ CAL 완료 및 모터 정지
```

## 9. GET OFF와 GET ON

CAL 완료 후 제어 코드가 목표 위치를 계산합니다.

```text
target = LOWER + (UPPER - LOWER) × percent / 100
```

- GET OFF: `0 + (100 - 0) × 10 / 100 = 10`
- GET ON: `0 + (100 - 0) × 50 / 100 = 50`

제어 코드는 현재 위치보다 목표가 크면 CW, 작으면 CCW를 명령합니다. SIL은 그 방향을 저장하고 `SIL_Tick()`마다 한 칸씩 이동하면서 Hall 인터럽트를 발생시킵니다.

## 10. 구현할 때 자주 발생하는 실수

1. 끝단에서도 Hall 인터럽트를 계속 호출함
   - 물리 위치는 멈췄지만 제어 위치만 계속 변합니다.

2. 모터 명령 함수에서 위치를 즉시 변경함
   - 주기적으로 움직이는 실제 모터 동작을 표현할 수 없습니다.

3. ADC 인터럽트를 Stall 상태에서만 호출함
   - 모터 정지 후 전류가 0으로 내려간 것을 제어 코드가 알 수 없습니다.

4. `control_current_raw`을 SIL에서 직접 수정함
   - 실제 코드와 SIL 코드가 전역변수를 공유하게 됩니다. 반드시 `SIL_ReadCurrentRaw()`와 인터럽트를 통해 전달합니다.

5. SIL 코드에 MCU 레지스터를 작성함
   - SIL은 PC에서 실행되므로 `ADC1->DR`, `GPIOA->BSRR`, `TIM2->CCR1` 같은 레지스터를 사용하지 않습니다.

## 11. 수강생 구현 확인표

- [ ] 초기 물리 위치가 50인가?
- [ ] CW에서 위치가 tick마다 `+1` 되는가?
- [ ] CCW에서 위치가 tick마다 `-1` 되는가?
- [ ] 실제 이동할 때만 `EXTI6_IRQHandler()`를 호출하는가?
- [ ] LOWER 0과 UPPER 100을 넘지 않는가?
- [ ] 정지 전류가 0인가?
- [ ] 정상 이동 전류가 1500인가?
- [ ] 끝단 구동 전류가 3200인가?
- [ ] 매 tick 마지막에 `ADC1_2_IRQHandler()`를 호출하는가?
- [ ] 제어 코드의 변수를 직접 참조하지 않는가?
- [ ] Windows의 `build-local.bat` 또는 Linux/macOS의 `make run`으로 빌드되는가?
- [ ] `code/main.c` 출력에서 CAL 완료와 목표 위치 값을 직접 확인했는가?
