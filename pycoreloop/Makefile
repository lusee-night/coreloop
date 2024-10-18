OBJ = lusee_commands.py lusee_appIds.py core_loop.py core_loop_errors.py


all: $(OBJ)

lusee_commands.py : ../documentation/lusee_commands.md  ../documentation/toplevel_commands.md ../scripts/create_cmd_include.py
	cd ..; python3 scripts/create_cmd_include.py >/dev/null

lusee_appIds.py : ../documentation/lusee_appIds.md ../scripts/create_appid_include.py
	cd ..; python3 scripts/create_appid_include.py >/dev/null

core_loop.py : ../coreloop/core_loop.h
	ctypesgen ../coreloop/core_loop.h >core_loop.py

core_loop_errors.py : ../coreloop/core_loop_errors.h
	ctypesgen ../coreloop/core_loop_errors.h >core_loop_errors.py

clean:
	rm -f $(OBJ)
