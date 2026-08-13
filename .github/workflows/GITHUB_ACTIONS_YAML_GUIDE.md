# GitHub Actions YAML 문법 정리 및 해석

이 문서는 이 프로젝트의 `.github/workflows/sil.yml`을 기준으로 YAML 기본 문법과 GitHub Actions 문법을 함께 설명합니다.

## 1. 전체 YAML 코드

```yaml
name: DC motor SIL build

on:
  push:
  pull_request:
  workflow_dispatch:

jobs:
  build-and-run:
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4

      - name: Build and run SIL
        run: make run
```

이 파일은 다음 위치에 있습니다.

```text
.github/workflows/sil.yml
```

## 2. 트리로 보는 전체 구조

```text
워크플로
│
├── name
│   └── DC motor SIL build
│
├── on
│   ├── push
│   ├── pull_request
│   └── workflow_dispatch
│
└── jobs
    │
    └── build-and-run
        │
        ├── runs-on
        │   └── ubuntu-latest
        │
        └── steps
            │
            ├── step 1
            │   └── uses
            │       └── actions/checkout@v4
            │
            └── step 2
                ├── name
                │   └── Build and run SIL
                │
                └── run
                    └── make run
```

읽는 방향은 위에서 아래, 바깥에서 안쪽입니다.

```text
워크플로
  └── 작업
        └── 단계
              └── 실행 내용
```

## 3. 순수 YAML 문법과 GitHub Actions 문법

YAML은 데이터를 계층적으로 작성하는 형식입니다. GitHub Actions는 YAML 파일 안에 어떤 키를 사용하고 어떤 동작을 수행할지 정해 둔 서비스입니다.

| 표현 | 종류 | 의미 |
|---|---|---|
| `key: value` | YAML | 키와 값의 관계 |
| 들여쓰기 | YAML | 상위 항목과 하위 항목의 관계 |
| `-` | YAML | 리스트의 한 항목 |
| `#` | YAML | 주석 |
| `name` | GitHub Actions | 워크플로 또는 단계의 표시 이름 |
| `on` | GitHub Actions | 워크플로 실행 조건 |
| `jobs` | GitHub Actions | 실행할 작업 목록 |
| `runs-on` | GitHub Actions | 작업을 실행할 운영체제 환경 |
| `steps` | GitHub Actions | 작업 안에서 실행할 단계 목록 |
| `uses` | GitHub Actions | 이미 만들어진 Action 사용 |
| `run` | GitHub Actions | 실행 환경의 쉘 명령어 실행 |

핵심 구분은 다음과 같습니다.

- `key: value`, 들여쓰기, `-`, `#`는 YAML의 기본 작성 방식입니다.
- `name`, `on`, `jobs`, `runs-on`, `steps`, `uses`, `run`은 GitHub Actions가 특별한 의미로 해석하는 키입니다.
- GitHub Actions는 YAML을 읽은 뒤 이 키들의 의미에 따라 작업을 실행합니다.

## 4. YAML 기본 문법

### 4.1 키와 값

```yaml
name: DC motor SIL build
```

- `name`은 키입니다.
- `DC motor SIL build`는 값입니다.
- 콜론 뒤에는 한 칸 띄우는 것이 일반적인 YAML 작성 방식입니다.

### 4.2 들여쓰기

```yaml
jobs:
  build-and-run:
    runs-on: ubuntu-latest
```

구조는 들여쓰기로 표현합니다.

```text
jobs
└── build-and-run
    └── runs-on
```

- `jobs`가 가장 바깥쪽 상위 항목입니다.
- `build-and-run`은 `jobs`의 하위 항목입니다.
- `runs-on`은 `build-and-run`의 하위 항목입니다.
- YAML에서는 탭보다 공백을 사용해야 합니다.
- 같은 단계에 있는 항목은 같은 수만큼 들여써야 합니다.

### 4.3 리스트 항목

```yaml
steps:
  - uses: actions/checkout@v4
  - name: Build and run SIL
    run: make run
```

`-`는 리스트의 한 항목을 뜻합니다. 위 예제에서 `steps`에는 두 개의 단계가 있습니다.

```text
steps
├── 첫 번째 항목
└── 두 번째 항목
```

두 번째 항목의 `name`과 `run`은 같은 단계에 속하므로 같은 깊이로 작성합니다.

### 4.4 주석

```yaml
# 이 줄은 설명이며 실행되지 않습니다.
name: DC motor SIL build
```

`#`부터 줄 끝까지는 주석입니다. 주석은 사람이 설정을 이해하도록 돕지만 GitHub Actions가 실행하지는 않습니다.

### 4.5 값이 비어 있는 키

```yaml
push:
pull_request:
workflow_dispatch:
```

콜론 뒤에 값이 없어도 문법 오류가 아닙니다. 이 프로젝트에서는 각 이벤트에 별도 조건을 지정하지 않고 기본 동작을 사용한다는 뜻입니다.

## 5. 이 프로젝트의 YAML 한 줄씩 해석하기

### 5.1 `name`

```yaml
name: DC motor SIL build
```

GitHub Actions 화면에 표시될 워크플로 이름입니다. 이름을 바꾸면 화면에 표시되는 제목만 바뀌고 빌드 동작은 바뀌지 않습니다.

### 5.2 `on`

```yaml
on:
  push:
  pull_request:
  workflow_dispatch:
```

워크플로를 언제 실행할지 정합니다.

| 이벤트 | 실행 시점 |
|---|---|
| `push` | 저장소에 커밋을 Push했을 때 |
| `pull_request` | Pull Request가 생성되거나 변경될 때 |
| `workflow_dispatch` | GitHub Actions 화면에서 수동 실행할 때 |

따라서 학생 브랜치에 Push하면 이 워크플로가 실행됩니다.

### 5.3 `jobs`

```yaml
jobs:
```

실행할 작업들의 묶음입니다. 하나의 워크플로에 여러 Job을 만들 수도 있지만, 이 프로젝트에는 하나의 Job만 있습니다.

### 5.4 Job ID: `build-and-run`

```yaml
jobs:
  build-and-run:
```

`build-and-run`은 Job의 내부 식별자입니다.

- 사람이 읽기 쉬운 이름으로 작성할 수 있습니다.
- 공백 대신 하이픈을 사용했습니다.
- GitHub Actions 화면에서 이 Job을 구분하는 데 사용됩니다.

### 5.5 `runs-on`

```yaml
runs-on: ubuntu-latest
```

이 Job을 실행할 컴퓨터 환경을 지정합니다.

이 프로젝트에서는 GitHub가 제공하는 최신 Ubuntu 환경을 사용합니다. 따라서 GitHub Actions에서는 Windows용 `build-local.bat`가 아니라 Ubuntu용 `Makefile` 명령이 실행됩니다.

### 5.6 `steps`

```yaml
steps:
```

Job 안에서 순서대로 실행할 단계들의 목록입니다.

이 프로젝트의 `steps`에는 다음 두 단계가 있습니다.

1. 저장소 코드 내려받기
2. C 코드 빌드 및 실행

### 5.7 `uses`

```yaml
- uses: actions/checkout@v4
```

이미 만들어진 Action을 사용하는 단계입니다.

- `actions/checkout`은 GitHub 저장소의 코드를 실행 환경으로 내려받는 공식 Action입니다.
- `@v4`는 사용할 Action 버전을 나타냅니다.
- 이 단계가 먼저 실행되어야 뒤의 `make run`이 프로젝트 파일을 찾을 수 있습니다.

`uses`는 직접 명령어를 작성하는 것이 아니라, 다른 사람이 만들어 둔 재사용 가능한 작업을 가져오는 문법입니다.

### 5.7.1 Checkout 전과 후의 작업 폴더

GitHub Actions가 Ubuntu 가상 PC를 준비했을 때 작업 폴더에는 프로젝트 파일이 자동으로 들어 있지 않습니다.

```text
Checkout 전
Ubuntu 가상 PC
└── 작업 폴더
    └── 거의 비어 있음
```

다음 단계가 실행되면 저장소의 코드가 작업 폴더로 내려받아집니다.

```yaml
- uses: actions/checkout@v4
```

이 프로젝트에서는 다음과 같은 구조가 만들어집니다.

```text
Checkout 후
Ubuntu 가상 PC
└── dc-motor-calibration-sil/
    ├── Makefile
    ├── code/
    │   ├── main.c
    │   ├── provided_control.c
    │   ├── sil_environment.c
    │   └── sil_api.h
    └── .github/
        └── workflows/
            └── sil.yml
```

프로젝트마다 폴더와 파일 이름은 다를 수 있습니다. 중요한 것은 `checkout`이 실행된 뒤에야 이 저장소의 `Makefile`과 C 소스 파일이 작업 폴더에 존재한다는 점입니다.

그래서 다음 단계가 실행될 수 있습니다.

```yaml
- name: Build and run SIL
  run: make run
```

실행 흐름은 다음과 같습니다.

```text
Ubuntu 작업 폴더
    ↓
actions/checkout@v4
    ↓
저장소 파일 내려받기
    ↓
Makefile과 code 폴더 생성
    ↓
make run 실행 가능
```

### 5.7.2 Checkout을 빼면 어떻게 되는가

`checkout` 단계를 제거하면 GitHub Actions가 저장소 코드를 작업 폴더에 내려받지 않습니다.

```yaml
steps:
  - name: Build and run SIL
    run: make run
```

이 상태에서는 `make run`이 읽을 `Makefile`과 C 소스 파일을 찾지 못합니다. 결과적으로 다음과 비슷한 오류가 발생할 수 있습니다.

```text
make: *** No targets specified and no makefile found.  Stop.
```

즉, `checkout`은 단순한 준비 단계가 아니라 **GitHub 저장소의 실제 코드를 실행 환경으로 가져오는 단계**입니다.

```text
checkout 없음
    ↓
작업 폴더에 프로젝트 파일 없음
    ↓
Makefile 없음
    ↓
make run 실패
```

따라서 일반적인 순서는 반드시 다음과 같아야 합니다.

```yaml
steps:
  # 1단계: 저장소 코드 내려받기
  - uses: actions/checkout@v4

  # 2단계: 내려받은 코드 빌드 및 실행
  - name: Build and run SIL
    run: make run
```

### 5.8 `name`과 `run`

```yaml
- name: Build and run SIL
  run: make run
```

`name`은 이 단계의 표시 이름입니다. GitHub Actions 실행 화면에 표시됩니다.

`run`은 실행 환경의 쉘에서 직접 실행할 명령어입니다.

```text
make run
```

이 명령은 프로젝트의 `Makefile`을 읽고 다음 순서로 동작합니다.

1. `code/provided_control.c`를 사용합니다.
2. 학생이 작성한 `code/sil_environment.c`를 사용합니다.
3. `code/main.c`를 사용합니다.
4. 세 파일을 컴파일합니다.
5. 만들어진 `dc_motor_sil`을 실행합니다.

## 6. `uses`와 `run`의 차이

| 문법 | 역할 | 이 프로젝트의 예 |
|---|---|---|
| `uses` | 이미 만들어진 Action 사용 | `actions/checkout@v4` |
| `run` | 쉘 명령어 직접 실행 | `make run` |

쉽게 구분하면 다음과 같습니다.

```text
uses = 완성된 도구를 가져와 사용
run  = 명령어를 직접 입력해 실행
```

## 7. GitHub Actions에서 실제로 일어나는 순서

```text
학생이 코드 수정
    ↓
git add
    ↓
git commit
    ↓
git push
    ↓
push 이벤트 발생
    ↓
GitHub Actions 워크플로 시작
    ↓
Ubuntu 실행 환경 준비
    ↓
actions/checkout이 저장소 코드 내려받기
    ↓
make run 실행
    ↓
C 코드 컴파일
    ↓
dc_motor_sil 실행
    ↓
CALIBRATION, GET OFF, GET ON, STOP 결과 확인
```

중요한 점은 **Commit하고 Push한 코드만 GitHub Actions에서 검사한다는 것**입니다. 컴퓨터에만 저장된 파일이나 Commit하지 않은 수정 내용은 Actions 실행 환경에 포함되지 않습니다.

## 8. 로컬 실행과 GitHub Actions 실행 비교

| 구분 | 로컬 Windows | 로컬 WSL | GitHub Actions |
|---|---|---|---|
| 실행 파일 | `build-local.bat` | `make run` | `make run` |
| 실행 환경 | Windows 또는 WSL 연결 | Ubuntu | GitHub Ubuntu |
| 목적 | Push 전 사전 확인 | Linux 방식 사전 확인 | Push된 코드 최종 확인 |
| 코드 위치 | 현재 저장소 폴더 | `/mnt/c/...` 또는 WSL 저장소 | checkout된 GitHub 저장소 |

## 9. 자주 하는 실수

### 실수 1. 들여쓰기 오류

잘못된 예:

```yaml
jobs:
build-and-run:
  runs-on: ubuntu-latest
```

`build-and-run`은 `jobs`의 하위 항목이어야 하므로 들여써야 합니다.

올바른 예:

```yaml
jobs:
  build-and-run:
    runs-on: ubuntu-latest
```

### 실수 2. `steps` 아래의 `-` 누락

잘못된 예:

```yaml
steps:
  uses: actions/checkout@v4
```

올바른 예:

```yaml
steps:
  - uses: actions/checkout@v4
```

`steps`는 여러 단계의 리스트이므로 각 단계 앞에 `-`가 필요합니다.

### 실수 3. Checkout 단계 누락

```yaml
steps:
  - name: Build and run SIL
    run: make run
```

실행 환경에 저장소 코드가 내려와 있지 않으면 `Makefile`과 C 파일을 찾지 못할 수 있습니다. 그래서 보통 `run` 단계보다 먼저 `actions/checkout`을 실행합니다.

### 실수 4. Windows 명령어를 Ubuntu에서 실행

GitHub Actions의 `runs-on`이 Ubuntu이므로 다음과 같이 Windows 명령어를 실행하면 안 됩니다.

```yaml
run: build-local.bat
```

이 프로젝트에서는 Ubuntu에서 동작하는 다음 명령을 사용합니다.

```yaml
run: make run
```

### 실수 5. 수정 파일을 Push하지 않음

로컬에서 파일을 수정했더라도 다음 과정이 없으면 GitHub Actions는 이전 코드를 검사합니다.

```bash
git add code/sil_environment.c
git commit -m "Implement SIL environment"
git push
```

## 10. 한 문장으로 요약

이 YAML은 **코드가 GitHub에 Push되면 Ubuntu 환경을 준비하고, 저장소를 내려받은 다음, `make run`으로 DC Motor SIL을 빌드하고 실행하는 GitHub Actions 워크플로**입니다.

## 참고 자료

- [GitHub Actions 워크플로 문법](https://docs.github.com/en/actions/reference/workflows-and-actions/workflow-syntax)
- [GitHub Actions 워크플로 개념](https://docs.github.com/en/actions/concepts/workflows-and-actions/workflows)
