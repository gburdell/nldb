# (A) (P)roc (H)andler
namespace eval aph {
	variable debug 0
	#Map of info by cmdName,opt.
	#For options, opt can be -switch | "*args*" for non-switch arg
	#and value is {'?'|'+'|'*'|'!' varNm?}.
	#Other keys (for opt) are *usage* and *help*.
	variable  infoByNameOpt	
	array unset infoByNameOpt
	variable  commands {}

	proc isObject {x} {
		nlsh _is_object $x
	}

	proc help {args} {
		variable commands
		puts "\nCommands:\n========="
		foreach i [lsort -increasing $commands] {
			puts $i
		}
		puts "[string repeat = 60]"
		puts "\n'<command> -help|-usage' for usage details of <command>.\n\n"
	}

	proc getUsage {cmd} {
		variable infoByNameOpt
		#TODO: fix to show better
		if {0} {
		#regsub -all {_} [join $infoByNameOpt(${cmd},*usage*)] { } usage
		} else {
			set usage $infoByNameOpt(${cmd},*usage*)
		}
		return "$cmd $usage"
	}

	proc addProc {args} {
		variable usageByName
		variable commands
		set cmdNm [lindex $args 0]
		if {0 <= [lsearch -exact $commands $cmdNm]} {
			error "${cmdNm}: command already exists"
		}
		lappend commands $cmdNm
		set args [lindex $args 1]
		updateInfo $cmdNm [lindex $args 0] [lindex $args 1]
		set body [lindex $args 2]
		#define the procedure
		if {{} != $body} {
			uplevel #0 proc $cmdNm args \{$body\}
		}
	}

	proc updateInfo {cmdNm usage help} {
		variable infoByNameOpt
		set infoByNameOpt(${cmdNm},*usage*) $usage
		set infoByNameOpt(${cmdNm},*help*)  $help
		set infoByNameOpt(${cmdNm},*required*) {}
		set infoByNameOpt(${cmdNm},*all*) {}
		#mechanize the usage
		foreach i $usage {
			if {[regexp {(^.+)(\?|\+|\*|\!)$} $i x m rpt]} {
				set i $m
			} else {
				set rpt {!}; #opt required marker
			}
			if {[regexp {(^\-[^:]+)(:(.+))?$} $i x opt x varNm]} {
				#switch: do nothing
			} else {
				#non-switch, so accumulate list of *args*
				set ix "${cmdNm},*args*"
				if {![info exists infoByNameOpt($ix)]} {
					set infoByNameOpt($ix) $rpt
				} else {
					set wasRpt [lindex $infoByNameOpt($ix) 0]
					if {$rpt != $wasRpt} {
						error "${cmdNm}: '${i}': specifies repeat: $rpt != $wasRpt"
					}
				}
				foreach {varNm dflt} [join [split $i {?=}]] {}
				lappend infoByNameOpt($ix) $varNm
				set opt $varNm
				if {{} != $dflt} {
					set infoByNameOpt(${cmdNm},${opt},*dflt*) $dflt
					lappend infoByNameOpt(${cmdNm},*required*) $varNm
				}
			}
			lappend infoByNameOpt(${cmdNm},*all*) $opt $varNm
			set ix "${cmdNm},${opt}"
			if {[info exists infoByNameOpt($ix)]} {
				error "${opt} already set for ${cmdNm}"
			}
			set infoByNameOpt($ix) "${rpt} ${varNm}"
			if {[regexp {\+|\!} $rpt]} {
				lappend infoByNameOpt(${cmdNm},*required*) $opt
			}
		}
		set infoByNameOpt(${cmdNm},*all*) [join $infoByNameOpt(${cmdNm},*all*)]
		debug 9 [array get infoByNameOpt "${cmdNm},*"]
	}

	proc debug {svr msg {body {}}} {
		variable debug
		if {$svr <= $debug} {
			set from [lindex [info level -1] 0]
			puts "DBG: ${from}: ${msg}"
			if {{} != $body} {
				uplevel $body
			}
		}
	}

	proc pop {fifoRef} {
		upvar $fifoRef fifo
		if {0 < [llength $fifo]} {
			set head [lindex $fifo 0]
			set fifo [lrange $fifo 1 end]
		} else {
			debug 1 "$fifo : [isObject $fifo]"
			#Note: else, _obj0x... vals become strings
			set head $fifo
			set fifo {}
		}
		return $head
	}

	#Disperse opts(v1_v2_v3) value {k1 k2 k3} to "set v1 k2", "set v2 k2", ...
	proc disperse {from} {
		uplevel {
			foreach [split $from _] $opts($from) {}
		}
	}

	proc checkExpr {expr aRef opRef rexRef {nil _}} {
		upvar $aRef attrName
		upvar $opRef op
		upvar $rexRef rex
		if {$expr == $nil} {
			nullify attrName op rex
		} elseif {1 > [regexp {(\S+)\s*(=\~|\!\~)\s*(\S+)$} \
		               $expr x attrName op rex]} {
			error "'${expr}': ill-formed expression"
		}
	}

	proc assert {cond} {
		if {0 == $cond} {
			error "assert failed"
		}
	}

	proc parseOpts {optsRef argsRef} {
		upvar $argsRef argv
		variable infoByNameOpt
		upvar $optsRef opts
		array unset opts; array set opts {}
		set cmdNm [lindex [info level -1] 0]
		set opts(*command*) $cmdNm
		debug 10 "1: ${cmdNm} : $argv : [llength $argv]"	
		if {[regexp {\-(help|usage)} [join $argv]]} {
			puts "
Usage: [getUsage ${cmdNm}]

$infoByNameOpt(${cmdNm},*help*)\n"
			return 1
		}
		#hash options we've done
		array set cntByOpt {}
		set expectArg 0
		while {{} != $argv} {
			set isObj [isObject [set ai [pop argv]]]
			set isOnlySwitch 0
			if {!$isObj && [regexp {^\-[a-zA-Z_][a-zA-Z_0-9]*} $ai]} {
				if {0 != $expectArg} {
					error "${cmdNm}: ${ai}: did not expect switch"
				}
				#a switch
				set ix "${cmdNm},${ai}"
				if {![info exists infoByNameOpt($ix)]} {
					error "${cmdNm}: ${ai}: invalid option"
				}
				set optVal {}
				set opts($ai) $ai
				set isOnlySwitch 1
			} else {
				#standalone arg
				set ix "${cmdNm},*args*"
				if {0 == $expectArg} {
					#1st time through, drop rpt
					set rpt [lindex $infoByNameOpt($ix) 0]
					set argVars [lrange $infoByNameOpt($ix) 1 end]
				}
				set xvarNm [pop argVars]
				if {($xvarNm != "") || ![info exists varNm]} {
					set varNm $xvarNm
				}
				debug 1 "6: $ai : $varNm"
				if {![info exists infoByNameOpt(${cmdNm},${varNm})]} {
					error "${ai}: unexpected argument"
				}
				set optVal $ai; #optVal is value to pass to varNm
				set ai2 [set ai $varNm]
				set expectArg 1; #only expect args, no switches
			}
			if {![info exists cntByOpt($ai)]} {set cntByOpt($ai) 0}
			incr cntByOpt($ai)
			#Process option to switch, if required
			debug 1 "5: $ai : $expectArg"
			if {0 == $expectArg} {
				foreach {rpt varNm} $infoByNameOpt($ix) {}
				if {{} != $varNm} {
					if {{} == $optVal} {
						set optVal [pop argv]
						if {{} == $optVal} {
							error "${ai}: option missing value for '${varNm}'"
						}
						set ai2 $ai
						set isOnlySwitch 0
					} else {
						set ai2 $varNm
					}
				} else {
					set varNm $ai
				}
			}
			if {0 == $isOnlySwitch} {
				if {[regexp {[\!\?]} $rpt]} {
					set opts(${varNm}) $optVal
				} else {
					lappend opts(${varNm}) $optVal
				}
			}
			assert [expr !$isObj || ($isObj && [isObject $optVal])]
			if {(1 < $cntByOpt($ai)) && [regexp {\?|\!} $rpt]} {
				error "${ai2}: option already specified"
			}
		}
		#make sure reqd opts are there
		foreach i $infoByNameOpt(${cmdNm},*required*) {
			if {![info exist cntByOpt($i)]} {
				set ix "${cmdNm},${i},*dflt*"
				if {[info exists infoByNameOpt($ix)]} {
					set opts($i) $infoByNameOpt($ix)
				} else {
					error "${cmdNm}: '${i}' option required"
				}
			}
		}
		transcribe opts 2
		debug 1 "3" {parray opts}
		return 0
	}

	#Transcribe opts values into caller as vars.  If no value,
	#set nil.
	proc transcribe {optsRef {lvl 1} {nil _}} {
		variable infoByNameOpt
		upvar $optsRef opts
		set cmd $opts(*command*)
		foreach i $infoByNameOpt(${cmd},*all*) {
			if {![regsub {^\-} $i {} varNm]} {
				set varNm $i
			}
			if {![info exists opts($i)]} {
				set opts($i) $nil
			}
			#evalute assign in caller
			uplevel $lvl "set $varNm \$opts($i)"
		}
	}

	proc nullify {args} {
		foreach i $args {
			uplevel set $i {_}
		}
	}
}

namespace eval nlsh::_priv {

proc helloWorld {} {

	puts "
#
# [get_tool_version]  [clock format [clock seconds]]
#
"
	if {0 != $::tcl_interactive} {
		puts {# nl_shell is an application which extends Tcl 8.4.
# Thus, all Tcl 8.4 commands are available, plus the addition
# of commands to support netlist manipulation.
# Type "help" for details.
}
	}
}
}

if {[regexp {/debug/} $::argv0]} {
	set ::tcl_prompt1 {puts -nonewline "nl_shell-g> "}
} else {
	set ::tcl_prompt1 {puts -nonewline "nl_shell> "}
}

#
# Add commands here
#

aph::addProc read_verilog {
	{fileName+}
	{Read verilog netlist(s) specified by 'fileName'(s).

Return TCL_OK on success; else TCL_ERROR if any parsing error(s).
}
	{
		if {[aph::parseOpts opts args]} {return}; #did help/usage
		#make fileName separate elements
		eval nlsh $opts(*command*) $fileName
	}
}

aph::addProc read_slf {
	{fileName+}
	{Read Synopsys Library File(s) specified by 'fileName'(s).

Return TCL_OK on success; else TCL_ERROR if any parsing error(s).
}
	{
		if {[aph::parseOpts opts args]} {return}; #did help/usage
		#make fileName separate elements
		eval nlsh $opts(*command*) $fileName
	}
}

aph::addProc get_tool_version {
	{}
	{Return nl_shell tool version.}
	{
		if {[aph::parseOpts opts args]} {return}; #did help/usage
		nlsh $opts(*command*); #no arguments
	}
}

aph::addProc current_design {
	designName?
	{Set current design, if 'designName' specified.  Return current design name.}
	{
		if {[aph::parseOpts opts args]} {return}; #did help/usage
		nlsh $opts(*command*) $designName
	}
}

aph::addProc link {
	{}
	{Link current design.
Return {} (empty list) on success.
Return flat list of {refNm1 n1 ... refNmn nn} indicating n unresolved
references, where each entry 'refNmi ni' is a pair indicating (unresolved)
reference name 'refNmi' and the number of instances referring to it 'ni'.
Such as list can be used directly to initialize a map of unresolved
reference count 'ni' by name 'refNmi', as in

array set unresolvedByName [link]
parray unresolvedByName
}
	{
		if {[aph::parseOpts opts args]} {return}; #did help/usage
		nlsh $opts(*command*)
	}
}

aph::addProc get_cells {
    {-hierarchical? -filter:fexpr? regex?=.+?}
    {Get cells of current design.
Return collection of cells.
Options are processed in the following order:

  -hierarchical  Creates collection of all cells found in depth-first
                 search of current design.

                 If -hierarchical is not specified, a collection of
                 only children cells of current design is created. 

  regex          A regular expression (default: .+) applied to the tail
                 name of all candidate cells (the cell set created
                 by use (or not) of the -hierarchical option).

  -filter fexpr  The filter expression 'fexpr' is applied to the resultant
                 set.  Cell with 'fexpr' true will be returned in final
                 collection.
}
    {
        if {[aph::parseOpts opts args]} {return}; #did help/usage
        aph::checkExpr $fexpr attrName op rex
        nlsh $opts(*command*) $hierarchical $filter $attrName $op $rex $regex
    }
}


proc is_object {x} {
	aph::isObject $x
}

proc help {args} {
	aph::help
}

nlsh::_priv::helloWorld
