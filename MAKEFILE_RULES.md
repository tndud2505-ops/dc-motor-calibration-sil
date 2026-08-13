# Makefile의 빌드 규칙 이해하기

## 1. Makefile은 위에서 아래로 실행하는 스크립트가 아니다

쉘 스크립트는 보통 명령을 위에서 아래로 실행합니다.

```sh
cc code/main.c -o program
./program
```

하지만 Makefile은 먼저 파일 전체를 읽고 **타깃과 의존성의 관계**를 파악합니다.
그다음 사용자가 요청한 타깃에 필요한 규칙만 실행합니다.

Makefile의 기본 규칙 구조는 다음과 같습니다.

```make
타깃: 의존성
	타깃을 만드는 명령
```

- `타깃`: 만들 파일 또는 수행할 작업의 이름
- `의존성`: 타깃보다 먼저 준비되어야 하는 파일이나 다른 타깃
- `명령`: 타깃을 실제로 만드는 방법이며 반드시 탭으로 시작

따라서 규칙이 Makefile 아래쪽에 있어도 Make가 파일 전체를 읽은 후 이름으로 찾아갑니다.

## 2. 현재 빌드 규칙을 변수 없이 풀어쓰기

현재 Makefile에는 다음 값이 들어 있습니다.

```make
CC ?= cc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Werror -pedantic
SOURCES := code/provided_control.c code/sil_environment.c code/main.c
BINARY := dc_motor_sil
```

이 변수들을 전부 실제 값으로 바꾸면 핵심 규칙은 다음과 같습니다.

```make
.PHONY: all run clean

dc_motor_sil: code/provided_control.c code/sil_environment.c code/main.c code/sil_api.h
	cc -std=c11 -O2 -Wall -Wextra -Werror -pedantic -Icode code/provided_control.c code/sil_environment.c code/main.c -o dc_motor_sil

all: dc_motor_sil



run: dc_motor_sil
	./dc_motor_sil

clean:
	-rm -f dc_motor_sil dc_motor_sil.exe *.o
```

여기서 가장 중요한 빌드 규칙은 다음 부분입니다.

```make
dc_motor_sil: code/provided_control.c code/sil_environment.c code/main.c code/sil_api.h
	cc -std=c11 -O2 -Wall -Wextra -Werror -pedantic -Icode code/provided_control.c code/sil_environment.c code/main.c -o dc_motor_sil
```

사람 말로 바꾸면 다음 뜻입니다.

> `dc_motor_sil`을 만들려면 C 소스 파일 3개와 `code/sil_api.h`가 필요하다.
> `dc_motor_sil`이 없거나 의존성보다 오래됐으면 아래의 `cc` 명령으로 다시 만든다.

Make는 `cc` 명령의 `-o dc_motor_sil`을 분석해서 결과 파일을 알아내는 것이 아닙니다.
콜론 왼쪽에 적힌 `dc_motor_sil`을 타깃으로 인식하고, 아래 명령이 그 타깃을 만든다고 믿습니다.

## 3. `all`과 `run`은 둘 다 같은 바이너리를 요구한다

변수를 풀면 두 규칙은 다음과 같습니다.

```make
all: dc_motor_sil

run: dc_motor_sil
	./dc_motor_sil
```

`all`과 `run`의 오른쪽에 모두 `dc_motor_sil`이 있습니다.
따라서 둘 중 어느 타깃을 요청해도 Make는 먼저 `dc_motor_sil`을 준비합니다.

```text
all ──┐
      ├──> dc_motor_sil 빌드 규칙
run ──┘
```

`run`이 `all`을 거치는 것은 아닙니다.
`all`과 `run`이 각각 직접 `dc_motor_sil`을 의존성으로 요구하는 구조입니다.

## 4. `make all`의 처리 과정

```sh
make all
```

Make는 다음 순서로 처리합니다.

1. `all` 규칙을 찾습니다.
2. `all`에 `dc_motor_sil`이 필요하다는 것을 확인합니다.
3. 이름이 `dc_motor_sil`인 빌드 규칙을 찾습니다.
4. 바이너리가 없거나 소스·헤더보다 오래됐으면 `cc` 명령을 실행합니다.
5. `dc_motor_sil`이 준비되면 `all`을 완료합니다.

`all`에는 실행 명령이 없으므로 프로그램을 실행하지 않고 빌드만 합니다.

## 5. `make run`의 처리 과정

```sh
make run
```

Make는 다음 순서로 처리합니다.

1. `run` 규칙을 찾습니다.
2. `run`에 `dc_motor_sil`이 필요하다는 것을 확인합니다.
3. 이름이 `dc_motor_sil`인 빌드 규칙을 찾습니다.
4. 바이너리가 없거나 소스·헤더보다 오래됐으면 `cc` 명령을 실행합니다.
5. 바이너리가 준비되면 `./dc_motor_sil`을 실행합니다.

즉, 바이너리가 없을 때의 흐름은 다음과 같습니다.

```text
make run
  → dc_motor_sil 없음
  → dc_motor_sil 빌드 규칙 실행
  → dc_motor_sil 생성
  → ./dc_motor_sil 실행
```

`./dc_motor_sil`은 실행만 담당합니다. 자동 빌드는 그 위의 다음 의존성 선언 때문에 일어납니다.

```make
run: dc_motor_sil
```

## 6. 바이너리가 이미 있으면 어떻게 되는가

Make는 타깃과 의존성 파일의 수정 시간을 비교합니다.

- `dc_motor_sil`이 없음: 빌드
- 소스 또는 헤더가 `dc_motor_sil`보다 최신임: 다시 빌드
- `dc_motor_sil`이 모든 의존성보다 최신임: 빌드 생략

따라서 `make all`에서 다음 메시지가 나올 수 있습니다.

```text
make: Nothing to be done for 'all'.
```

이 메시지는 오류가 아닙니다. `dc_motor_sil`이 이미 최신이라 다시 빌드할 필요가 없다는 뜻입니다.

`make run`에서는 바이너리가 최신이면 빌드는 생략하지만 다음 명령은 실행합니다.

```sh
./dc_motor_sil
```

`run`이 `.PHONY`로 선언되어 있기 때문에 `make run`을 요청할 때마다 실행 작업을 수행합니다.

## 7. 핵심 정리

```text
make all
  → dc_motor_sil을 최신 상태로 준비
  → 필요하면 빌드
  → 실행하지 않음

make run
  → dc_motor_sil을 최신 상태로 준비
  → 필요하면 빌드
  → dc_motor_sil 실행
```

가장 중요한 점은 다음 세 가지입니다.

1. Makefile은 위에서 아래로 모든 줄을 실행하지 않습니다.
2. `all`과 `run`은 각각 `dc_motor_sil`을 직접 요구합니다.
3. `dc_motor_sil: ...` 규칙이 바이너리를 언제, 어떻게 빌드할지 선언합니다.
