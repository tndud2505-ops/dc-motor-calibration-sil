# 로컬 빌드 후 GitHub 제출 가이드

이 문서는 `build-local.bat` 또는 `build-local.sh` 실행에 성공한 뒤 진행합니다. Git과 SIL을 한 번에 배우지 않고, 먼저 로컬 C 빌드를 확인한 다음 GitHub 제출을 연습합니다.

## 1. GitHub 계정 만들기

1. https://github.com/ 접속
2. `Sign up` 선택
3. 개인 계정 생성
4. 이메일 인증 완료
5. 강사에게 자신의 GitHub 아이디 전달

계정 생성에 대한 공식 안내는 [GitHub 계정 생성 문서](https://docs.github.com/get-started/start-your-journey/creating-an-account-on-github)를 참고합니다.

## 2. Collaborator 초대 수락

강사가 수강생을 `dc-motor-calibration-sil` 저장소의 Collaborator로 초대합니다. 수강생은 GitHub 알림 또는 이메일에서 초대를 수락합니다.

초대를 수락해야 강사 저장소에 자신의 브랜치를 Push할 수 있습니다.

## 3. Git 설치

Git이 없다면 https://git-scm.com/downloads 에서 자신의 운영체제에 맞는 Git을 설치합니다.

설치 후 PowerShell 또는 Git Bash에서 확인합니다.

```bash
git --version
```

버전 번호가 출력되면 설치가 완료된 것입니다.

## 4. Git 사용자 정보 설정

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

## 5. 강사 저장소 Clone

작업할 상위 폴더에서 다음 명령을 실행합니다.

```bash
git clone https://github.com/tndud2505-ops/dc-motor-calibration-sil.git
cd dc-motor-calibration-sil
```

Clone은 GitHub에 있는 저장소의 현재 파일과 이력을 자신의 PC로 복사하는 작업입니다. [GitHub Clone 공식 문서](https://docs.github.com/repositories/creating-and-managing-repositories/cloning-a-repository)

## 6. 학생 전용 브랜치 만들기

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

## 7. `sil_environment.c` 작성 및 로컬 확인

수정 대상은 다음 파일 하나입니다.

```text
sil_environment.c
```

Windows에서는 다음 명령으로 다시 확인합니다.

```powershell
.\build-local.bat
```

Linux 또는 macOS에서는 다음을 실행합니다.

```bash
sh build-local.sh
```

CAL, GET OFF, GET ON, STOP 출력이 끝까지 나온 다음 Git에 기록합니다.

## 8. 변경 파일 확인

```bash
git status
```

`sil_environment.c`가 변경 파일로 표시되는지 확인합니다. `dc_motor_sil` 또는 `dc_motor_sil.exe`는 `.gitignore`에 등록되어 있으므로 제출 파일에 포함되지 않습니다.

## 9. Add와 Commit

수정한 SIL 파일을 Commit 대상으로 선택합니다.

```bash
git add sil_environment.c
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

## 10. 강사 저장소에 Push

```bash
git push -u origin student/학번
```

예시는 다음과 같습니다.

```bash
git push -u origin student/20260001
```

처음 Push할 때 GitHub 로그인을 요구할 수 있습니다. Git Credential Manager의 브라우저 로그인 화면이 나타나면 자신이 만든 GitHub 계정으로 로그인합니다.

Push는 로컬 Commit을 GitHub 저장소의 학생 브랜치로 전송하는 작업입니다.

## 11. GitHub Actions 결과 확인

1. https://github.com/tndud2505-ops/dc-motor-calibration-sil 접속
2. 상단 `Actions` 선택
3. 자신의 `student/학번` 브랜치에서 실행된 최신 항목 선택
4. `build-and-run` 선택
5. `Build and run SIL` 로그 확인

GitHub Actions는 Docker 없이 GitHub가 제공하는 Ubuntu 실행 환경에서 다음 작업을 수행합니다.

```text
provided_control.c + sil_environment.c + main.c 컴파일
→ 실행
→ CAL / GET OFF / GET ON / STOP 변수 출력
```

초록색 체크가 표시되면 빌드와 실행이 완료된 것입니다. 빨간색 X가 표시되면 `Build and run SIL` 로그의 첫 번째 C 컴파일 오류를 확인합니다. [GitHub Actions 실행 기록 확인 문서](https://docs.github.com/actions/monitoring-and-troubleshooting-workflows/monitoring-workflows/viewing-workflow-run-history)

## 12. 수정 후 다시 제출

한 번 Push한 뒤 코드를 다시 고쳤다면 같은 브랜치에서 다음 명령을 반복합니다.

```bash
git add sil_environment.c
git commit -m "SIL 환경 수정"
git push
```

Push할 때마다 GitHub Actions가 다시 실행됩니다.

## 제출 확인표

- [ ] GitHub 이메일 인증을 완료했는가?
- [ ] Collaborator 초대를 수락했는가?
- [ ] 현재 브랜치가 `student/자신의학번`인가?
- [ ] 수정 파일이 `sil_environment.c` 하나인가?
- [ ] 로컬 빌드가 끝까지 실행되는가?
- [ ] Commit을 만들었는가?
- [ ] 학생 브랜치로 Push했는가?
- [ ] GitHub Actions 로그를 확인했는가?
