geneMQ.exe: geneMQ.obj alocbuf2d.obj alocbuf.obj erout.obj hamwdw1.obj segmsn.obj get_cmd.obj commandanal2.obj cmd_hp.obj sysparsetCom.obj cmd_pkedit.obj systemcall.obj fpeakinwdw1.obj ordr.obj fmagmeanfd3.obj wavshpcoef.obj polyval.obj
	bcc32 -egeneMQ.exe geneMQ.obj alocbuf2d.obj alocbuf.obj erout.obj hamwdw1.obj segmsn.obj get_cmd.obj commandanal2.obj cmd_hp.obj sysparsetCom.obj cmd_pkedit.obj systemcall.obj fpeakinwdw1.obj ordr.obj fmagmeanfd3.obj wavshpcoef.obj polyval.obj

#when revised, below are compiled.
geneMQ.obj: geneMQ.c sinupktNew.h sinupktfrtr.h sinuedPar.h
	bcc32 -c -w-8057 -w-8004 -w-8060 geneMQ.c

alocbuf2d.obj: alocbuf2d.c
	bcc32 -c -w-8057 -w-8004 alocbuf2d.c

alocbuf.obj: alocbuf.c
	bcc32 -c -w-8057 -w-8004 alocbuf.c

erout.obj: erout.c
        bcc32 -c -w-8057 -w-8004 erout.c

hamwdw1.obj: hamwdw1.c
	bcc32 -c -w-8057 -w-8004 hamwdw1.c

segmsn.obj: segmsn.c
	bcc32 -c -w-8057 -w-8004 segmsn.c

get_cmd.obj: get_cmd.c
	bcc32 -c -w-8057 -w-8004 get_cmd.c

commandanal2.obj: commandanal2.c
	bcc32 -c -w-8057 -w-8004 commandanal2.c

cmd_hp.obj: cmd_hp.c
	bcc32 -c -w-8057 -w-8004 cmd_hp.c

sysparsetCom.obj: sysparsetCom.c
	bcc32 -c -w-8057 -w-8004 sysparsetCom.c

cmd_pkedit.obj: cmd_pkedit.c
	bcc32 -c -w-8057 -w-8004 cmd_pkedit.c

systemcall.obj: systemcall.c
	bcc32 -c -w-8057 -w-8004 systemcall.c

fpeakinwdw1.obj: fpeakinwdw1.c
	bcc32 -c -w-8057 -w-8004 fpeakinwdw1.c

ordr.obj: ordr.c
	bcc32 -c -w-8057 -w-8004 ordr.c

fmagmeanfd3.obj: fmagmeanfd3.c
	bcc32 -c -w-8057 -w-8004 fmagmeanfd3.c

wavshpcoef.obj: wavshpcoef.c
	bcc32 -c -w-8057 -w-8004 wavshpcoef.c

polyval.obj: polyval.c
	bcc32 -c -w-8057 -w-8004 polyval.c

genesinu.obj: genesinu.c
	bcc32 -c -w-8057 -w-8004 genesinu.c
