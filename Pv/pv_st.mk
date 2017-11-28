pv_st.exe: pv_st.obj alocbuf2d.obj alocbuf.obj erout.obj hamwdw1.obj lpfdsn1.obj fsreadpvr.obj wdwsgnlpv.obj segmsn.obj d1fft.obj wavRopen.obj wavWopen.obj wavWclose.obj
	bcc32 -epv_st.exe pv_st.obj alocbuf2d.obj alocbuf.obj erout.obj hamwdw1.obj lpfdsn1.obj fsreadpvr.obj wdwsgnlpv.obj segmsn.obj d1fft.obj wavRopen.obj wavWopen.obj wavWclose.obj

#when revised, below are compiled.
pv_st.obj: pv_st.c pv.h
	bcc32 -c pv_st.c

alocbuf2d.obj: alocbuf2d.c
	bcc32 -c alocbuf2d.c

alocbuf.obj: alocbuf.c
	bcc32 -c alocbuf.c

erout.obj: erout.c
        bcc32 -c erout.c

hamwdw1.obj: hamwdw1.c
	bcc32 -c hamwdw1.c

lpfdsn1.obj: lpfdsn1.c
	bcc32 -c lpfdsn1.c

fsreadpvr.obj: fsreadpvr.c
	bcc32 -c fsreadpvr.c

wdwsgnlpv.obj: wdwsgnlpv.c
	bcc32 -c wdwsgnlpv.c

segmsn.obj: segmsn.c
	bcc32 -c segmsn.c

d1fft.obj: d1fft.c
	bcc32 -c d1fft.c

wavRopen.obj: wavRopen.c wavlib.h
	bcc32 -c wavRopen.c

wavWopen.obj: wavWopen.c wavlib.h
	bcc32 -c wavWopen.c

wavWclose.obj: wavWclose.c wavlib.h
	bcc32 -c wavWclose.c
