cd c:\e\bin\linux
copy ssie7up ssie7up_7.2.a
copy ssie7 ssie_v_7.2.a
copy mft mft_v_7.2.a
..\windows\ssie7up.exe -add ssie7up_7.2.a ssie_v_7.2.a
..\windows\ssie7up.exe -add ssie7up_7.2.a mft_v_7.2.a
..\windows\ssie7up.exe -add ssie7up_7.2.a c:\e\src\ssie7\pkg\linux\logiciels\ssie7\mft
..\windows\ssie7up.exe -add ssie7up_7.2.a c:\e\src\ssie7\pkg\linux\logiciels\ssie7\mftcat
..\windows\ssie7up.exe -list ssie7up_7.2.a
copy ssie7up_7.2.a c:\e\pkg\ssie7up_7.2.a.5_Linux

pause
