CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
TARGETS = daemon_manager my_daemon

.PHONY: all clean install uninstall run help

all: $(TARGETS)
	@echo "✅ Build completed successfully!"
	@echo "🚀 Run './daemon_manager' to start the manager"

daemon_manager: daemon_launcher.c
	@echo "🔨 Compiling daemon manager..."
	$(CC) $(CFLAGS) -o $@ $<
	@echo "✅ Daemon manager compiled"

my_daemon: my_daemon.c
	@echo "🔨 Compiling daemon process..."
	$(CC) $(CFLAGS) -o $@ $<
	@echo "✅ Daemon process compiled"

clean:
	@echo "🧹 Cleaning build files..."
	rm -f $(TARGETS)
	rm -f /tmp/daemon_pids.txt
	rm -f /tmp/temp_daemon.txt
	@echo "✅ Clean completed"

install: all
	@echo "📦 Installing to /usr/local/bin..."
	sudo cp daemon_manager /usr/local/bin/
	sudo cp my_daemon /usr/local/bin/
	sudo chmod +x /usr/local/bin/daemon_manager
	sudo chmod +x /usr/local/bin/my_daemon
	@echo "✅ Installation completed"

uninstall:
	@echo "🗑️  Uninstalling..."
	sudo rm -f /usr/local/bin/daemon_manager
	sudo rm -f /usr/local/bin/my_daemon
	@echo "✅ Uninstallation completed"

run: all
	@echo "🚀 Starting Daemon Manager..."
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	./daemon_manager

debug: CFLAGS += -g -DDEBUG
debug: all
	@echo "🐛 Debug build completed"

check-logs:
	@echo "📋 Recent daemon logs:"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@sudo tail -20 /var/log/syslog | grep -E "(enhanced_daemon|my_daemon)" || echo "No recent logs found"

kill-all:
	@echo "⚡ Killing all managed daemons..."
	@if [ -f /tmp/daemon_pids.txt ]; then \
		while read pid; do \
			if kill -0 $$pid 2>/dev/null; then \
				echo "Killing PID $$pid"; \
				kill -TERM $$pid; \
			fi; \
		done < /tmp/daemon_pids.txt; \
		rm -f /tmp/daemon_pids.txt; \
		echo "✅ All daemons terminated"; \
	else \
		echo "No daemon registry found"; \
	fi

help:
	@echo "🎯 Enhanced Daemon Manager - Build System"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "Available targets:"
	@echo "  📦 all         - Build all components"
	@echo "  🚀 run         - Build and run daemon manager"
	@echo "  🐛 debug       - Build with debug symbols"
	@echo "  🧹 clean       - Remove build files"
	@echo "  📋 check-logs  - Show recent daemon logs"
	@echo "  ⚡ kill-all    - Terminate all managed daemons"
	@echo "  🔧 install     - Install to system (requires sudo)"
	@echo "  🗑️  uninstall  - Remove from system (requires sudo)"
	@echo "  ❓ help        - Show this help message"
	@echo ""
	@echo "📝 Usage examples:"
	@echo "  make run       # Quick start"
	@echo "  make debug     # Debug build"
	@echo "  make clean all # Clean rebuild"