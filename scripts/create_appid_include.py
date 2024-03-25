# we will convert the ../documentation/commands.md to a C header file
#
#
of = open("include/lusee_appIds.h", "w")
of.write("""
#ifndef LUSEE_APPIDS_H
#define LUSEE_APPIDS_H

"""
)

for line in open("documentation/lusee_appIds.md"):
    line = line.split('|')
   
    if len(line)<5:
        continue
    print (line, len(line))
    offset, name, priority,  desc = line[1:5]
    offset = offset.strip().replace('x','0')
    name = name.strip()
    desc = desc.strip()
    if len(desc)==0:
        desc = "No description available"
    if desc[-1]=='\n':
        desc = desc[:-1]
    #print (offset, name, desc)
    if "AppID" not in name:
        continue
    of.write (f"// {desc}; Priority: {priority}\n")
    of.write(f"#define {name} 0x02{offset.strip()}\n\n")
    

of.write("\n\n#endif")