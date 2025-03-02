##
## EPITECH PROJECT, 2025
## Makefile
## File description:
## it makes files
##

# Variables
BUILD_DIR := build
CMAKE_FLAGS := -DCMAKE_BUILD_TYPE=Release

# Rules
all: $(BUILD_DIR)/Makefile
	@$(MAKE) -j -C $(BUILD_DIR)

$(BUILD_DIR)/Makefile: CMakeLists.txt | $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake $(CMAKE_FLAGS) ..

clean: $(BUILD_DIR)
	@$(MAKE) -C $(BUILD_DIR) clean 2>/dev/null || true

fclean: clean
	@rm -rf $(BUILD_DIR)

re: fclean all

# Phony targets
.PHONY: all clean fclean re

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
