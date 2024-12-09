#-- MAKEFILE CONFIG -----------------------------------------------------------
MAKEFLAGS		:= --no-print-directory
.DEFAULT_GOAL	:= all
.DELETE_ON_ERROR:

#-- PROJECT CONFIG ------------------------------------------------------------
NAME		:= mod1
SRCS_DIR	:= srcs
BUILD_DIR	:= .build
NB_THREAD	:= 4

#-- FILES ---------------------------------------------------------------------
SRCS	:=	srcs/engine/inputs/InputManager.cpp \
			srcs/engine/inputs/Key.cpp \
			srcs/engine/inputs/Mouse.cpp \
			srcs/engine/maths/Random.cpp \
			srcs/engine/maths/Vec2.cpp \
			srcs/engine/maths/Vec3.cpp \
			srcs/engine/render/Mesh.cpp \
			srcs/engine/render/Shader.cpp \
			srcs/main.cpp \
			srcs/parsing.cpp \

OBJS	:= ${SRCS:$(SRCS_DIR)/%.cpp=$(BUILD_DIR)/%.o}
DEPS	:= ${SRCS:$(SRCS_DIR)/%.cpp=$(BUILD_DIR)/%.d}
DIRS	:= $(sort $(shell dirname $(OBJS)))

#-- COMPILATION ---------------------------------------------------------------
CC				:= g++
CXXFLAGS		:= -I$(SRCS_DIR) -MP -MMD -g
OPENGL_FLAGS	:= -lglfw -lGLEW -lGL

#-- COLORS --------------------------------------------------------------------
NOC			:= \e[0m
BOLD		:= \e[1m
UNDERLINE	:= \e[4m
BLACK		:= \e[1;30m
RED			:= \e[1m\e[38;5;196m
GREEN		:= \e[1m\e[38;5;76m
YELLOW		:= \e[1m\e[38;5;220m
BLUE		:= \e[1m\e[38;5;33m
PURPLE		:= \e[1;35m
CYAN		:= \e[1;36m
WHITE		:= \e[1;37m

#-- COMPTER UTILS -------------------------------------------------------------
NB_COMPILE	:= 1
NB_OBJS		:= $(words $(OBJS))
ifndef RECURSION
TO_COMPILE	:= $(shell var=$$(make -n RECURSION=42 | grep compile | wc -l); if [ $$var -eq 0 ]; then echo $(NB_OBJS); else echo $$var; fi;)
else
TO_COMPILE	:= 1
endif
PERCENTAGE	:= 0

#-- RULES ---------------------------------------------------------------------

$(DIRS):
	@mkdir -p $(@)

$(BUILD_DIR)/%.o: $(SRCS_DIR)/%.cpp | $(DIRS)
	@if [ $(NB_COMPILE) -eq 1 ]; then echo "$(BLUE)Compiliation of $(TO_COMPILE) sources $(NOC)"; fi
	$(eval PERCENTAGE=$(shell expr $(NB_COMPILE)00 '/' $(TO_COMPILE)))
	@printf "  %3i%% - $(PURPLE)compile $<$(NOC)\n" $(PERCENTAGE)
	@$(CC) $(CXXFLAGS) -o $@ -c $< $(OPENGL_FLAGS)
	$(eval NB_COMPILE=$(shell expr $(NB_COMPILE) + 1))

$(NAME): $(OBJS)
	@echo "$(BLUE)Linking$(NOC)"
	@$(CC) $(CXXFLAGS) -o $@ $^ $(OPENGL_FLAGS)
	@echo "$(GREEN)Done !$(NOC)"

all:
	@make -j$(NB_THREAD) $(NAME)

clean:
	@echo "$(RED)Remove objects$(NOC)"
	@rm -rf $(NAME)

fclean: clean
	@echo "$(RED)Remove binary$(NOC)"
	@rm -rf $(BUILD_DIR)

re: fclean
	@make -j$(NB_THREAD) $(NAME)

run: $(NAME)
	@echo "$(BLUE)start $(NAME) !$(NOC)"
	@./$< map/testmap.mod1
	@echo "$(BLUE)bye bye :)$(NOC)"

runval: $(NAME)
	@echo "$(BLUE)start $(NAME) !$(NOC)"
	@valgrind --error-limit=no --suppressions=vsupp ./$< map/testmap.mod1
	@echo "$(BLUE)bye bye :)$(NOC)"

runvalall: $(NAME)
	@echo "$(BLUE)start $(NAME) !$(NOC)"
	@valgrind --error-limit=no --suppressions=vsupp --leak-check=full --show-leak-kinds=all ./$< map/testmap.mod1
	@echo "$(BLUE)bye bye :)$(NOC)"

install:
	@echo "$(BLUE)You need to have sudo permission$(NOC)"
	@sudo apt-get install libglfw3 libglfw3-dev libglew-dev

.PHONY: all clean fclean re run runval runvalall install

-include $(DEPS)
