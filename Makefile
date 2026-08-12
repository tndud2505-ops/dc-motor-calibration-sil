# 사용할 C 컴파일러를 지정합니다.
# CC가 이미 설정되어 있으면 기존 값을 사용합니다.
CC ?= cc

# C 컴파일 옵션을 지정합니다.
# C11 표준, 최적화, 모든 경고 활성화, 경고를 오류로 처리,
# 엄격한 표준 준수를 의미합니다.
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Werror -pedantic

# 컴파일할 소스 파일 목록입니다.
SOURCES := code/provided_control.c code/sil_environment.c code/main.c

# 생성할 실행 파일의 이름입니다.
BINARY := dc_motor_sil

# 아래 이름들은 실제 파일이 아니라 Make의 작업 이름임을 지정합니다.
.PHONY: all run clean

# 기본 작업입니다.
# 별도의 작업을 지정하지 않고 make만 실행하면 all이 실행됩니다.
all: $(BINARY)

# 실행 파일을 만들기 위한 규칙입니다.
# 소스 파일이나 헤더 파일이 변경되면 다시 빌드합니다.
$(BINARY): $(SOURCES) code/sil_api.h
	# C 컴파일러로 소스 파일들을 컴파일하고 실행 파일을 생성합니다.
	$(CC) $(CFLAGS) -Icode $(SOURCES) -o $(BINARY)

# 실행 파일이 없으면 먼저 실행 파일을 빌드한 뒤 실행합니다.
run: $(BINARY)
	# 생성된 SIL 실행 파일을 실행합니다.
	./$(BINARY)

# 빌드 결과물을 삭제하는 작업입니다.
clean:
	# 실행 파일과 오브젝트 파일을 삭제합니다.
	# 명령 앞의 '-'는 삭제할 파일이 없어도 오류를 무시한다는 뜻입니다.
	# $(RM)은 Make에서 기본적으로 rm -f로 설정되어 있습니다.
	-$(RM) $(BINARY) $(BINARY).exe *.o
