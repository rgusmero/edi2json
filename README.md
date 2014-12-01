edi2json
========

Convert EDIFACT to "raw" JSON

A tiny utility to convert EDIFACT to "raw" JSON. With "raw" we mean that the final JSON is composed of "arrays of arrays of (arrays of)  strings". Higher level EDIFACT must be managed afterwards.

Requirements:
   - libedi 1.0.1+ <a href="http://code.google.com/p/libedi/">http://code.google.com/p/libedi/</a>
   - json module from CCAN <a href="http://ccodearchive.net/info/json.html">http://ccodearchive.net/info/json.html</a>

Example usage:

<pre>
edi2json edi.txt json.txt
</pre>

with edi.txt (from Wikipedia):

<pre>
UNA:+.? '
UNB+UNOB:2+8022628000004:14+4306517008994:14+050805:0941+8++DESADV+++EANCOM'
UNH+1+DESADV:D:96A:UN:EAN005'
BGM+351::9+007774+9'
DTM+137:20050714:102'
DTM+11:20050805:102'
DTM+2:20050714:102'
RFF+DQ:007774'
RFF+ON:509671432'
NAD+SU+8022628000004::9'
NAD+BY+4306517005092::9'
NAD+DP+4306517005078::9'
CPS+1'
LIN+1++8022628900014:EN'
PIA+1+90001:SA+5082951:BY'
IMD+F++:::COMP.SAFARI 65 C/SPECCH.MOLA'
QTY+12:66.00:PCE'
QTY+21:66.00:PCE'
CNT+2:1'
UNT+18+1'
UNZ+1+8'
</pre>

produces json.txt:

<pre>
[
	[
		[
			"\nUNB"
		],
		[
			"UNOB",
			"2"
		],
		[
			"8022628000004",
			"14"
		],
		[
			"4306517008994",
			"14"
		],
		[
			"050805",
			"0941"
		],
		[
			"8"
		],
		[
			""
		],
		[
			"DESADV"
		],
		[
			""
		],
		[
			""
		],
		[
			"EANCOM"
		]
	],
	[
		[
			"\nUNH"
		],
		[
			"1"
		],
		[
			"DESADV",
			"D",
			"96A",
			"UN",
			"EAN005"
		]
	],
	....
</pre>
(please note the "\n" before "UNB")

License: 3-caluse BSD

Author: Riccardo Gusmeroli - rgusmero@elet.polimi.it

	

