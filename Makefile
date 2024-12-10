.PHONY: all clean reinstall libmx

# Directories
LIBMX_DIR = resources/libmx
CLIENT_DIR = client
SERVER_DIR = server

# Targets
all: libmx uchat uchat_server

libmx:
	@echo "Building libmx..."
	@$(MAKE) -sC $(LIBMX_DIR)

uchat: libmx
	@echo "Building uchat..."
	@$(MAKE) -sC $(CLIENT_DIR)

uchat_server: libmx
	@echo "Building uchat_server..."
	@$(MAKE) -sC $(SERVER_DIR)

clean:
	@$(MAKE) -sC $(LIBMX_DIR) clean
	@$(MAKE) -sC $(CLIENT_DIR) clean
	@$(MAKE) -sC $(SERVER_DIR) clean
	@rm -f uchat uchat_server

reinstall: clean
	@$(MAKE) -sC $(LIBMX_DIR) reinstall
	@$(MAKE) -sC $(CLIENT_DIR) reinstall
	@$(MAKE) -sC $(SERVER_DIR) reinstall
	@mv client/uchat .
	@mv server/uchat_server .
