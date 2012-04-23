/* KAAPI/KaapiC interface interface */
// -----------------------------------------
// by Fabien Le Mentec and Thierry Gautier
//(c) INRIA, projet MOAIS, 2011
// See Copyright file.
/********************************************************
 WARNING! This file has been automatically generated by M4
 samedi 21 avril 2012, 01:12:22 (UTC+0200)
 *******************************************************/
#define KAAPIC_MAX_ARGS 32
#define KAAPIC_ADAPTIVE_SWITCH_ULL(__w, __i, __j, __tid) \
switch((__w)->nargs) { \
 case 0: (__w)->u.f_c_ull(__i, __j, __tid  ); break; \
 case 1: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0] ); break; \
 case 2: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1] ); break; \
 case 3: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2] ); break; \
 case 4: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3] ); break; \
 case 5: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4] ); break; \
 case 6: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5] ); break; \
 case 7: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6] ); break; \
 case 8: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7] ); break; \
 case 9: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8] ); break; \
 case 10: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9] ); break; \
 case 11: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9], (__w)->args[10] ); break; \
 case 12: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9], (__w)->args[10], (__w)->args[11] ); break; \
 case 13: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9], (__w)->args[10], (__w)->args[11], (__w)->args[12] ); break; \
 case 14: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9], (__w)->args[10], (__w)->args[11], (__w)->args[12], (__w)->args[13] ); break; \
 case 15: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9], (__w)->args[10], (__w)->args[11], (__w)->args[12], (__w)->args[13], (__w)->args[14] ); break; \
 case 16: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9], (__w)->args[10], (__w)->args[11], (__w)->args[12], (__w)->args[13], (__w)->args[14], (__w)->args[15] ); break; \
 case 17: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9], (__w)->args[10], (__w)->args[11], (__w)->args[12], (__w)->args[13], (__w)->args[14], (__w)->args[15], (__w)->args[16] ); break; \
 case 18: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9], (__w)->args[10], (__w)->args[11], (__w)->args[12], (__w)->args[13], (__w)->args[14], (__w)->args[15], (__w)->args[16], (__w)->args[17] ); break; \
 case 19: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9], (__w)->args[10], (__w)->args[11], (__w)->args[12], (__w)->args[13], (__w)->args[14], (__w)->args[15], (__w)->args[16], (__w)->args[17], (__w)->args[18] ); break; \
 case 20: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9], (__w)->args[10], (__w)->args[11], (__w)->args[12], (__w)->args[13], (__w)->args[14], (__w)->args[15], (__w)->args[16], (__w)->args[17], (__w)->args[18], (__w)->args[19] ); break; \
 case 21: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9], (__w)->args[10], (__w)->args[11], (__w)->args[12], (__w)->args[13], (__w)->args[14], (__w)->args[15], (__w)->args[16], (__w)->args[17], (__w)->args[18], (__w)->args[19], (__w)->args[20] ); break; \
 case 22: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9], (__w)->args[10], (__w)->args[11], (__w)->args[12], (__w)->args[13], (__w)->args[14], (__w)->args[15], (__w)->args[16], (__w)->args[17], (__w)->args[18], (__w)->args[19], (__w)->args[20], (__w)->args[21] ); break; \
 case 23: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9], (__w)->args[10], (__w)->args[11], (__w)->args[12], (__w)->args[13], (__w)->args[14], (__w)->args[15], (__w)->args[16], (__w)->args[17], (__w)->args[18], (__w)->args[19], (__w)->args[20], (__w)->args[21], (__w)->args[22] ); break; \
 case 24: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9], (__w)->args[10], (__w)->args[11], (__w)->args[12], (__w)->args[13], (__w)->args[14], (__w)->args[15], (__w)->args[16], (__w)->args[17], (__w)->args[18], (__w)->args[19], (__w)->args[20], (__w)->args[21], (__w)->args[22], (__w)->args[23] ); break; \
 case 25: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9], (__w)->args[10], (__w)->args[11], (__w)->args[12], (__w)->args[13], (__w)->args[14], (__w)->args[15], (__w)->args[16], (__w)->args[17], (__w)->args[18], (__w)->args[19], (__w)->args[20], (__w)->args[21], (__w)->args[22], (__w)->args[23], (__w)->args[24] ); break; \
 case 26: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9], (__w)->args[10], (__w)->args[11], (__w)->args[12], (__w)->args[13], (__w)->args[14], (__w)->args[15], (__w)->args[16], (__w)->args[17], (__w)->args[18], (__w)->args[19], (__w)->args[20], (__w)->args[21], (__w)->args[22], (__w)->args[23], (__w)->args[24], (__w)->args[25] ); break; \
 case 27: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9], (__w)->args[10], (__w)->args[11], (__w)->args[12], (__w)->args[13], (__w)->args[14], (__w)->args[15], (__w)->args[16], (__w)->args[17], (__w)->args[18], (__w)->args[19], (__w)->args[20], (__w)->args[21], (__w)->args[22], (__w)->args[23], (__w)->args[24], (__w)->args[25], (__w)->args[26] ); break; \
 case 28: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9], (__w)->args[10], (__w)->args[11], (__w)->args[12], (__w)->args[13], (__w)->args[14], (__w)->args[15], (__w)->args[16], (__w)->args[17], (__w)->args[18], (__w)->args[19], (__w)->args[20], (__w)->args[21], (__w)->args[22], (__w)->args[23], (__w)->args[24], (__w)->args[25], (__w)->args[26], (__w)->args[27] ); break; \
 case 29: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9], (__w)->args[10], (__w)->args[11], (__w)->args[12], (__w)->args[13], (__w)->args[14], (__w)->args[15], (__w)->args[16], (__w)->args[17], (__w)->args[18], (__w)->args[19], (__w)->args[20], (__w)->args[21], (__w)->args[22], (__w)->args[23], (__w)->args[24], (__w)->args[25], (__w)->args[26], (__w)->args[27], (__w)->args[28] ); break; \
 case 30: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9], (__w)->args[10], (__w)->args[11], (__w)->args[12], (__w)->args[13], (__w)->args[14], (__w)->args[15], (__w)->args[16], (__w)->args[17], (__w)->args[18], (__w)->args[19], (__w)->args[20], (__w)->args[21], (__w)->args[22], (__w)->args[23], (__w)->args[24], (__w)->args[25], (__w)->args[26], (__w)->args[27], (__w)->args[28], (__w)->args[29] ); break; \
 case 31: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9], (__w)->args[10], (__w)->args[11], (__w)->args[12], (__w)->args[13], (__w)->args[14], (__w)->args[15], (__w)->args[16], (__w)->args[17], (__w)->args[18], (__w)->args[19], (__w)->args[20], (__w)->args[21], (__w)->args[22], (__w)->args[23], (__w)->args[24], (__w)->args[25], (__w)->args[26], (__w)->args[27], (__w)->args[28], (__w)->args[29], (__w)->args[30] ); break; \
 case 32: (__w)->u.f_c_ull(__i, __j, __tid , (__w)->args[0], (__w)->args[1], (__w)->args[2], (__w)->args[3], (__w)->args[4], (__w)->args[5], (__w)->args[6], (__w)->args[7], (__w)->args[8], (__w)->args[9], (__w)->args[10], (__w)->args[11], (__w)->args[12], (__w)->args[13], (__w)->args[14], (__w)->args[15], (__w)->args[16], (__w)->args[17], (__w)->args[18], (__w)->args[19], (__w)->args[20], (__w)->args[21], (__w)->args[22], (__w)->args[23], (__w)->args[24], (__w)->args[25], (__w)->args[26], (__w)->args[27], (__w)->args[28], (__w)->args[29], (__w)->args[30], (__w)->args[31] ); break; \
default: kaapi_abort(); break ;\
}
