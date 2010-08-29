# SConstruct
# Balance construction file
# Author: Joe Riedel (joeriedel@hotmail.com)

build_switches_override = None
variant_override = '../'

SConscript('./Radiance/SConscript', ['build_switches_override', 'variant_override'])

Import('radvars')
(build, source) = radvars

variant_dir = build.variantDir('Balance')
exe_type = 'EXE'
SConscript(
	'Source/SConstruct', 
	variant_dir='Bin/Intermediate/Balance' + build.targetDir(), 
	duplicate=0,
	exports=['variant_dir', 'exe_type']
)

if build.win() and build.tools(): # build .com variant on windows for command line stuff.
	variant_dir = build.variantDir('BalanceCom')
	exe_type = 'COM'
	SConscript(
		'Source/SConstruct', 
		variant_dir='Bin/Intermediate/BalanceCom' + build.targetDir(), 
		duplicate=0,
		exports=['variant_dir', 'exe_type']
	)