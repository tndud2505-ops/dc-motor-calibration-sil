# target: 실제 MCU 환경(FW/BSW)

이 폴더는 실제 Virtual MCU/STM32 실행 환경에만 사용합니다.

- GPIO, TIM2, ADC1, USART1, EXTI6, Hall 레지스터를 설정합니다.
- 전류 기반 Stopper, Hall pulse, 모터 CW/CCW/STOP callback을 제공합니다.
- `common/` 제어 로직을 호출하지만 학생 제출 대상은 아닙니다.
