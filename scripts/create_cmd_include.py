# we will convert the ../documentation/commands.md to a C header file
#
#
of = open("include/lusee_settings.h", "w")
of.write("""
#ifndef LUSEE_SETTINGS_H
#define LUSEE_SETTINGS_H

// Master mode command for which everything below applies.
#define RFS_Settings 0x10

"""
)

for line in open("documentation/lusee_commands.md"):
    line = line.split('|')
    if len(line)<4:
        continue
    offset, name, desc = line[1:4]
    name = name.strip()
    desc = desc.strip()
    if desc[-1]=='\n':
        desc = desc[:-1]
    #print (offset, name, desc)
    if offset[:3] != " 0x":
        continue
    if "RFS_SET" not in name:
        continue
    of.write (f"// {desc}\n")
    of.write(f"#define {name} {offset}\n\n")
    print (f'            case {name}:\n                cdi_not_implemented("{name}");\n                return;')

of.write("\n\n#endif")