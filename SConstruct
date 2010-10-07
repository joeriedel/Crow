# SConstruct
# Crow construction file
# Author: Joe Riedel (joeriedel@hotmail.com)

import os

project_name = 'Crow'
build_switches_override = None
variant_override = '../'
architecture = None

SConscript('./Radiance/SConscript', ['build_switches_override', 'variant_override', 'architecture'])

Import('radvars')
(build, source) = radvars

variant_dir = build.variantDir(project_name)
exe_type = 'EXE'
output = SConscript(
	'Source/SConstruct', 
	variant_dir='Bin/Intermediate/' + project_name + build.targetDir(), 
	duplicate=0,
	exports=['variant_dir', 'exe_type', 'project_name']
)

if build.switches.ios_device() and build.switches.ios_universal_binary():
# first pass built default architecture
# build other one

	# gather generated libs
	Import('lua')
	Import('boost_threads')
	Import('radiance_runtime')
	Import('radiance_main')
	Import('radiance_engine')
	
	lua_libs = [lua]
	boost_thread_libs = [boost_threads]
	radiance_runtime_libs = [radiance_runtime]
	radiance_main_libs = [radiance_main]
	radiance_engine_libs = [radiance_engine[1]]
	
	other = 'armv6'
	if build.switches.armv6():
		architecture = 'armv7'
	else:
		other = 'armv7'
		architecture = 'armv6'
	
	SConscript('./Radiance/SConscript', ['build_switches_override', 'variant_override', 'architecture'])
	
	Import('radvars')
	(build, source) = radvars

	variant_dir = build.variantDir(project_name)
	exe_type = 'EXE'
	output2 = SConscript(
		'Source/SConstruct', 
		variant_dir='Bin/Intermediate/' + project_name + build.targetDir(), 
		duplicate=0,
		exports=['variant_dir', 'exe_type', 'project_name']
	)
	
	# gather generated libs
	Import('lua')
	Import('boost_threads')
	Import('radiance_runtime')
	Import('radiance_main')
	Import('radiance_engine')
	
	lua_libs.append(lua)
	boost_thread_libs.append(boost_threads)
	radiance_runtime_libs.append(radiance_runtime)
	radiance_main_libs.append(radiance_main)
	radiance_engine_libs.append(radiance_engine[1])
	
	def makeUB(name, libs):
		p = build.backend.makeIOSUniversalBinary(source, name, [other, architecture], os.path.abspath('.'), 'Bin/Intermediate/' + project_name + '/' + build.backend.target() , libs)
		source.Install(build.__absTargetDir__(), p)
		
	makeUB('lib' + project_name + '.a', [output, output2])
	makeUB('libLua.a', lua_libs)
	makeUB('libboost_threads.a', boost_thread_libs)
	makeUB('libRuntime.a', radiance_runtime_libs)
	makeUB('libMain.a', radiance_main_libs)
	makeUB('libEngine.a', radiance_engine_libs)
	
if build.win() and build.tools(): # build .com variant on windows for command line stuff.
	variant_dir = build.variantDir(project_name + 'Com')
	exe_type = 'COM'
	SConscript(
		'Source/SConstruct', 
		variant_dir='Bin/Intermediate/' + project_name + 'Com' + build.targetDir(), 
		duplicate=0,
		exports=['variant_dir', 'exe_type', 'project_name']
	)
