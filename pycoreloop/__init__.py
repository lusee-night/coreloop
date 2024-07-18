from . import core_loop as struct

from . import lusee_commands as command
command_from_value, value_from_command = {}, {}
for name in dir(command):
    if name[0]=="_":
        continue
    value = getattr(command, name)
    command_from_value [value] = name
    value_from_command [name] = value

from . import lusee_appIds as appId
appId_from_value, value_from_appId = {}, {}
for name in dir(appId):
    if name[0]=="_":
        continue
    value = getattr(appId, name)
    appId_from_value [value] = name
    value_from_appId [name] = value


