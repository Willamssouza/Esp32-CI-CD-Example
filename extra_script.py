Import("env")

my_flags = env.ParseFlags(env['BUILD_FLAGS'])
defines = {k: v for (k, v) in my_flags.get("CPPDEFINES")}

version_file = open(".pio/build/esp32dev/last_version.txt", "w+")
version_file.write(defines.get("VERSION"))
version_file.close()

env.Replace(PROGNAME="firmware_%s" % defines.get("VERSION"))
