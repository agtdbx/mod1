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
SRCS	:= srcs/main.cpp
OBJS	:= ${SRCS:$(SRCS_DIR)/%.cpp=$(BUILD_DIR)/%.o}
DEPS	:= ${SRCS:$(SRCS_DIR)/%.cpp=$(BUILD_DIR)/%.d}
DIRS	:= $(sort $(shell dirname $(OBJS)))

#-- COMPILATION ---------------------------------------------------------------
CC				:= g++
CXXFLAGS		:= -I$(SRCS_DIR)
OPENGL_FLAGS	:= -lglfw -lGLEW -lGL

#-- COLORS --------------------------------------------------------------------
NOC			= \e[0m
BOLD		= \e[1m
UNDERLINE	= \e[4m
BLACK		= \e[1;30m
RED			= \e[1m\e[38;5;196m
GREEN		= \e[1m\e[38;5;76m
YELLOW		= \e[1m\e[38;5;220m
BLUE		= \e[1m\e[38;5;33m
PURPLE		= \e[1;35m
CYAN		= \e[1;36m
WHITE		= \e[1;37m

#-- COMPTER UTILS -------------------------------------------------------------
NB_COMPILE	:= 1
ifndef RECURSION
TO_COMPILE	:= $(shell var=$$(make -n RECURSION=42 | grep compile | wc -l); if [ $$var -eq 0 ]; then echo 1; else echo $$var; fi;)
else
TO_COMPILE	:= 1
endif
PERCENTAGE	:= 0

#-- RULES ---------------------------------------------------------------------

$(DIRS):
	@mkdir -p $(@)

$(BUILD_DIR)/%.o: $(SRCS_DIR)/%.cpp | $(DIRS)
	@if [ $(NB_COMPILE) -eq 1 ]; then echo "$(BLUE)Compiliation of $(TO_COMPILE) sources $(NOC)"; fi
	$(eval PERCENTAGE=$(shell expr $(NB_COMPILE)00 "/" $(TO_COMPILE)))
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
	@./$<
	@echo "$(BLUE)bye bye :)$(NOC)"

runval: $(NAME)
	@echo "$(BLUE)start $(NAME) !$(NOC)"
	@valgrind --suppressions=vsupp ./$<
	@echo "$(BLUE)bye bye :)$(NOC)"

runvalall: $(NAME)
	@echo "$(BLUE)start $(NAME) !$(NOC)"
	@valgrind --suppressions=vsupp --leak-check=full --show-leak-kinds=all ./$<
	@echo "$(BLUE)bye bye :)$(NOC)"

install:
	@echo "$(BLUE)You need to have sudo permission$(NOC)"
	@sudo apt-get install libglew-dev

.PHONY: all clean fclean re run runval runvalall install

-include $(DEPS)
