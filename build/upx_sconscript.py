# UPX builder for stubs

Import("env")
upx = WhereIs("upx")
def UPX_builder(target, source, env):
	"""If an nvgt_windows.bin stub starts with the MZ header, some sort of extra antivirus scanning takes place when a user tries compiling an nvgt script which makes such a compilation take a slightly longer time to complete. To fix that we replace the first 2 bytes of stubs with 'NV', but that causes UPX to not recognise the file. If we tell scons to mutate the stubs after UPX has been applied, then UPX will execute again on the next scons run because the mutation of the stubs makes scons think that nvgt_windows.bin was changed at the point/time in the build graph where UPX runs. The solution, thus this function instead of scons's command builder, is to make sure that if a source to be run through UPX starts with NV we temporarily replace it with MZ before running UPX and replacing it back with NV, thus UPX is happy and scons doesn't see any apparent changes. I am thoroughly displeased with this solution, but cannot find any other way even when messing with the various scons .Depends, .Ignore, .Requires functions and friends."""
	replace_with_nv = []
	for s in source:
		with open(str(s), "rb+") as f:
			f.seek(0)
			if f.read(2) != b"NV": continue
			replace_with_nv.append(s)
			f.seek(0)
			f.write(b"MZ")
	r = env.Execute(f"\"{upx}\" --best -q -o \"{target[0]}\" \"{source[0]}\"" + (">nul" if env["PLATFORM"] == "win32" else ">/dev/null"))
	if r: return r
	for s in replace_with_nv:
		with open(str(s), "rb+") as f:
			f.seek(0)
			f.write(b"NV")

if upx and ARGUMENTS.get("no_upx", "1") != "1":
	env["upx"] = UPX_builder
	env.Append(BUILDERS = {"UPX": Builder(action = UPX_builder)})
