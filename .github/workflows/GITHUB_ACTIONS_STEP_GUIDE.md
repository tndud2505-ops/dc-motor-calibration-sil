# GitHub Actions 캡처와 YML 매칭

이 문서는 캡처에 보이는 `build-and-run` 실행 화면을 `.github/workflows/sil.yml`의 항목과 연결해 설명합니다.

## 캡처와 연결되는 현재 YML

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

## 화면 요소가 보이는 이유

| 캡처 화면 요소 | 연결되는 YML 요소 | 왜 보이는가 |
|---|---|---|
| `build-and-run` | `jobs:` 아래의 `build-and-run:` | job의 식별자입니다. GitHub가 이 job 이름을 화면 상단에 표시합니다. |
| `succeeded 7 minutes ago in 3s` | 특정 한 줄이 아니라 job 실행 결과 | 모든 단계가 오류 코드 없이 끝났기 때문에 GitHub가 `succeeded`와 전체 실행 시간을 자동으로 표시합니다. |
| `Set up job` | `runs-on: ubuntu-latest` | GitHub가 작업을 실행할 Ubuntu runner를 준비하면서 자동으로 표시합니다. YML에 직접 작성한 `steps`는 아닙니다. |
| `Run actions/checkout@v4` | `- uses: actions/checkout@v4` | 저장소 파일을 runner로 내려받는 checkout action이 실행되므로 표시됩니다. |
| `Build and run SIL` | `- name: Build and run SIL` | 이 문자열이 화면의 단계 이름으로 그대로 표시됩니다. |
| `Build and run SIL`의 실행 내용 | `run: make run` | 해당 단계에서 `make run` 명령을 실행합니다. `Makefile`을 읽어 C 코드를 빌드하고 SIL 실행 파일을 실행합니다. |
| `Post Run actions/checkout@v4` | `actions/checkout@v4`의 사후 처리 | YML에 직접 작성하지 않았지만 checkout action이 끝난 뒤 인증 정보와 Git 설정을 정리하면서 GitHub가 자동으로 표시합니다. |
| Post Run 안의 `Node 20 is being deprecated` | checkout action의 실행 환경 안내 | 현재 action이 사용하는 Node 버전 관련 안내입니다. 우리 YML의 `run: make run` 로그가 아닙니다. |
| Post Run 안의 `git config`, `extraheader`, `HOME` 관련 명령 | checkout action의 정리 작업 | checkout 중 임시로 설정한 Git 인증·환경 설정을 삭제하거나 되돌리는 자동 명령입니다. |
| `Complete job` | YML에 직접 대응하는 항목 없음 | 모든 단계를 끝낸 뒤 GitHub가 결과와 로그를 정리하면서 자동으로 표시합니다. |
| 각 단계 오른쪽의 `0s`, `1s` | YML 한 줄이 아님 | GitHub가 각 단계에 걸린 시간을 자동으로 측정해 표시합니다. |

## YML 항목을 바꾸면 화면이 어떻게 달라지는가

| YML 변경 | 캡처 화면의 변화 |
|---|---|
| `runs-on: ubuntu-latest` 삭제 | 실행할 runner를 정할 수 없어 job이 시작되지 않습니다. 따라서 `Set up job` 이후 단계도 실행되지 않습니다. |
| `- uses: actions/checkout@v4` 삭제 | `Run actions/checkout@v4`와 그 사후 처리인 `Post Run actions/checkout@v4`가 보이지 않습니다. 저장소 파일을 내려받지 않으므로 `make run`도 실패할 수 있습니다. |
| `- name: Build and run SIL`의 이름 변경 | 화면의 해당 단계 이름도 변경한 문자열로 바뀝니다. |
| `run: make run` 삭제 또는 변경 | SIL 빌드·실행 명령이 달라집니다. 명령이 실패하면 해당 단계에 빨간색 `X`가 표시됩니다. |
| `jobs.build-and-run` 이름 변경 | 화면 상단의 `build-and-run` job 이름이 변경됩니다. |

## 캡처에 직접 나타나지 않는 YML

`name: DC motor SIL build`는 workflow 이름입니다. `push`, `pull_request`, `workflow_dispatch`는 workflow를 언제 실행할지 정하며, 캡처의 단계 목록이 아니라 실행을 시작하는 조건입니다.

정리하면, 캡처의 `Set up job`·`Post Run actions/checkout@v4`·`Complete job`은 GitHub가 자동으로 만들고, `Run actions/checkout@v4`와 `Build and run SIL`은 YML의 `steps`에 직접 정의한 내용입니다.
