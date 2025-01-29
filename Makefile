MESON_CONFIG_DIR	:= build
MESON_BUILD_DIR		:= release
EXECUTABLE_NAME		:= mod1
DEFAULT_MAP			:= map/flat.mod1
# DEFAULT_MAP			:= map/crater.mod1
# DEFAULT_MAP			:= map/42.mod1
# DEFAULT_MAP			:= map/escalier.mod1
DEFAULT_MAP			:= map/test.mod1


all: $(EXECUTABLE_NAME)

$(EXECUTABLE_NAME): build
	ninja -C $(MESON_CONFIG_DIR)
	ninja install -C $(MESON_CONFIG_DIR) >/dev/null

build:
	meson $(MESON_CONFIG_DIR) --prefix=$$PWD/$(MESON_BUILD_DIR) --bindir="" --libdir=""

clean:
	rm -rf $(MESON_BUILD_DIR)

fclean: clean
	rm -rf $(MESON_CONFIG_DIR)

re: fclean all

run: all
	cd $(MESON_BUILD_DIR) && ./$(EXECUTABLE_NAME) $(DEFAULT_MAP) || echo "Exit at 1"

runval: all
	cd $(MESON_BUILD_DIR) && valgrind --suppressions=vsupp ./$(EXECUTABLE_NAME) $(DEFAULT_MAP) || echo "Exit at 1"

runvalall: all
	cd $(MESON_BUILD_DIR) && valgrind --suppressions=vsupp --leak-check=full --show-leak-kinds=all ./$(EXECUTABLE_NAME) $(DEFAULT_MAP) || echo "Exit at 1"

.PHONY: all clean fclean re run runval runvalall
