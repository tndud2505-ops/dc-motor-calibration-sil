# 로컬 빌드 후 GitHub 제출 가이드

이 문서는 `build-local.bat` 또는 `build-local.sh` 실행에 성공한 뒤 진행합니다. Git과 SIL을 한 번에 배우지 않고, 먼저 로컬 C 빌드를 확인한 다음 GitHub 제출을 연습합니다.

## 1. GitHub 계정 만들기

1. https://github.com/ 접속
2. `Sign up` 선택
3. 개인 계정 생성
4. 이메일 인증 완료
5. 로그인 후 오른쪽 위 프로필 아이콘을 눌러 자신의 GitHub 아이디 확인

계정 생성에 대한 공식 안내는 [GitHub 계정 생성 문서](https://docs.github.com/get-started/start-your-journey/creating-an-account-on-github)를 참고합니다.

## 2. GitHub 아이디를 강사에게 카카오톡으로 전달

GitHub 가입과 이메일 인증을 완료한 뒤 강사에게 다음 형식으로 카카오톡 메시지를 보냅니다.

```text
SIL 과제 GitHub 아이디 전달
학번: 20260001
이름: 홍길동
GitHub 아이디: hong-gildong
```

GitHub 아이디는 프로필에 표시되는 `@아이디` 또는 프로필 주소의 마지막 부분입니다.

```text
프로필 주소: https://github.com/hong-gildong
GitHub 아이디: hong-gildong
```

표시 이름, 이메일 주소가 아니라 정확한 GitHub 아이디를 보냅니다. 비밀번호, 인증번호, Personal Access Token은 강사에게 보내지 않습니다.

강사가 초대를 찾지 못하면 다음 항목을 다시 확인합니다.

- 영문 대소문자와 하이픈을 정확히 적었는가?
- 프로필 주소가 실제로 열리는가?
- 이메일 인증을 완료했는가?

## 3. Collaborator 초대 수락

강사가 수강생을 `dc-motor-calibration-sil` 저장소의 Collaborator로 초대합니다. 수강생은 GitHub 알림 또는 이메일에서 초대를 수락합니다.

초대를 수락해야 강사 저장소에 자신의 브랜치를 Push할 수 있습니다.

1. GitHub에 로그인합니다.
2. 오른쪽 위 알림 종 모양을 확인하거나 초대 이메일을 엽니다.
3. `View invitation` 또는 저장소 초대 알림을 선택합니다.
4. `Accept invitation`을 선택합니다.
5. https://github.com/tndud2505-ops/dc-motor-calibration-sil 주소가 열리는지 확인합니다.

초대가 보이지 않으면 카카오톡으로 보낸 GitHub 아이디가 정확한지 먼저 확인한 뒤 강사에게 재확인을 요청합니다.

## 4. Git 설치

Git이 없다면 https://git-scm.com/downloads 에서 자신의 운영체제에 맞는 Git을 설치합니다.

설치 후 PowerShell 또는 Git Bash에서 확인합니다.

```bash
git --version
```

버전 번호가 출력되면 설치가 완료된 것입니다.

## 5. Git 사용자 정보 설정

자신의 이름과 GitHub에서 인증한 이메일을 설정합니다.

```bash
git config --global user.name "홍길동"
git config --global user.email "본인의_GitHub_이메일"
```

설정값을 확인합니다.

```bash
git config --global user.name
git config --global user.email
```

## 6. 강사 저장소 Clone

작업할 상위 폴더에서 다음 명령을 실행합니다.

```bash
git clone https://github.com/tndud2505-ops/dc-motor-calibration-sil.git
cd dc-motor-calibration-sil
```

Clone은 GitHub에 있는 저장소의 현재 파일과 이력을 자신의 PC로 복사하는 작업입니다. [GitHub Clone 공식 문서](https://docs.github.com/repositories/creating-and-managing-repositories/cloning-a-repository)

## 7. 학생 전용 브랜치 만들기

`학번` 부분을 실제 학번으로 바꿉니다.

```bash
git switch -c student/학번
```

예시는 다음과 같습니다.

```bash
git switch -c student/20260001
```

현재 브랜치를 확인합니다.

```bash
git branch --show-current
```

반드시 `student/자신의학번`이 출력되어야 합니다. `main`에 직접 Push하지 않습니다.

## 8. `code/sil_environment.c` 작성 및 로컬 확인

수정 대상은 다음 파일 하나입니다.

```text
code/sil_environment.c
```

Windows에서는 다음 명령으로 다시 확인합니다.

```powershell
.\build-local.bat
```

Linux 또는 macOS에서는 다음을 실행합니다.

```bash
sh build-local.sh
```

CAL, GET OFF, GET ON, STOP 출력이 끝까지 나오는지 확인합니다. 구현을 완료한 제출에서는 CAL 완료, GET OFF 10, GET ON 50 값도 함께 확인한 다음 Git에 기록합니다.

## 9. 변경 파일 확인

```bash
git status
```

`code/sil_environment.c`가 변경 파일로 표시되는지 확인합니다. `dc_motor_sil` 또는 `dc_motor_sil.exe`는 `.gitignore`에 등록되어 있으므로 제출 파일에 포함되지 않습니다.

## 10. Add와 Commit

수정한 SIL 파일을 Commit 대상으로 선택합니다.

```bash
git add code/sil_environment.c
```

Commit을 생성합니다.

```bash
git commit -m "학번 SIL 환경 제출"
```

예시는 다음과 같습니다.

```bash
git commit -m "20260001 SIL 환경 제출"
```

- `git add`: 다음 Commit에 포함할 파일 선택
- `git commit`: 선택한 변경 내용을 하나의 버전으로 저장

## 11. 강사 저장소에 Push

```bash
git push -u origin student/학번
```

예시는 다음과 같습니다.

```bash
git push -u origin student/20260001
```

처음 Push할 때 GitHub 로그인을 요구할 수 있습니다. Git Credential Manager의 브라우저 로그인 화면이 나타나면 자신이 만든 GitHub 계정으로 로그인합니다.

Push는 로컬 Commit을 GitHub 저장소의 학생 브랜치로 전송하는 작업입니다.

## 12. GitHub Actions 결과 확인

1. https://github.com/tndud2505-ops/dc-motor-calibration-sil 접속
2. 상단 `Actions` 선택
3. 자신의 `student/학번` 브랜치에서 실행된 최신 항목 선택
4. `build-and-run` 선택
5. `Build and run SIL` 로그 확인

GitHub Actions는 Docker 없이 GitHub가 제공하는 Ubuntu 실행 환경에서 다음 작업을 수행합니다.

```text
code/provided_control.c + code/sil_environment.c + code/main.c 컴파일
→ 실행
→ CAL / GET OFF / GET ON / STOP 변수 출력
```

초록색 체크가 표시되면 빌드와 실행이 완료된 것입니다. 이것은 SIL 기능 정답을 뜻하지 않습니다. 로그의 CAL, GET OFF, GET ON 값도 직접 확인해야 합니다. 빨간색 X가 표시되면 `Build and run SIL` 로그의 첫 번째 C 컴파일 오류를 확인합니다. [GitHub Actions 실행 기록 확인 문서](https://docs.github.com/actions/monitoring-and-troubleshooting-workflows/monitoring-workflows/viewing-workflow-run-history)

## 13. 수정 후 다시 제출

한 번 Push한 뒤 코드를 다시 고쳤다면 같은 브랜치에서 다음 명령을 반복합니다.

```bash
git add code/sil_environment.c
git commit -m "SIL 환경 수정"
git push
```

Push할 때마다 GitHub Actions가 다시 실행됩니다.

## 제출 확인표

- [ ] GitHub 이메일 인증을 완료했는가?
- [ ] 학번, 이름, 정확한 GitHub 아이디를 카카오톡으로 보냈는가?
- [ ] Collaborator 초대를 수락했는가?
- [ ] 현재 브랜치가 `student/자신의학번`인가?
- [ ] 수정 파일이 `code/sil_environment.c` 하나인가?
- [ ] 로컬 빌드가 끝까지 실행되는가?
- [ ] Commit을 만들었는가?
- [ ] 학생 브랜치로 Push했는가?
- [ ] GitHub Actions 로그를 확인했는가?
