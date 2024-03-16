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
    if desc[-1]=='\n':
        desc = desc[:-1]
    print (offset, name, desc)
    if offset[:3] != " 0x":
        continue
    if "RFS_SET" not in name:
        continue
    of.write (f"// {desc}\n")
    of.write(f"#define {name} {offset}\n\n")

of.write("\n\n#endif")