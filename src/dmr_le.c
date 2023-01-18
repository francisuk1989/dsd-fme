/*-------------------------------------------------------------------------------
 * dmr_le.c
 * DMR Late Entry MI Fragment Assembly, Procesing, and related Alg functions
 *
 * LWVMOBILE
 * 2022-12 DSD-FME Florida Man Edition
 *-----------------------------------------------------------------------------*/

#include "dsd.h"

//gather ambe_fr mi fragments for processing
void dmr_late_entry_mi_fragment (dsd_opts * opts, dsd_state * state, uint8_t vc, char ambe_fr[4][24], char ambe_fr2[4][24], char ambe_fr3[4][24])
{
  
  uint8_t slot = state->currentslot;
  
  //collect our fragments and place them into storage
  state->late_entry_mi_fragment[slot][vc][0] = (uint64_t)ConvertBitIntoBytes(&ambe_fr[3][0], 4);
  state->late_entry_mi_fragment[slot][vc][1] = (uint64_t)ConvertBitIntoBytes(&ambe_fr2[3][0], 4);
  state->late_entry_mi_fragment[slot][vc][2] = (uint64_t)ConvertBitIntoBytes(&ambe_fr3[3][0], 4);

  if (vc == 6) dmr_late_entry_mi (opts, state);

}

void dmr_late_entry_mi (dsd_opts * opts, dsd_state * state)
{
  uint8_t slot = state->currentslot;
  int i, j;
  int g[3]; 
  unsigned char mi_go_bits[24]; 

  uint64_t mi_test = 0;
  uint64_t go_test = 0;
  uint64_t mi_corrected = 0;
  uint64_t go_corrected = 0;
  
  mi_test = (state->late_entry_mi_fragment[slot][1][0] << 32L) | (state->late_entry_mi_fragment[slot][2][0] << 28) | (state->late_entry_mi_fragment[slot][3][0] << 24) |
            (state->late_entry_mi_fragment[slot][1][1] << 20) | (state->late_entry_mi_fragment[slot][2][1] << 16) | (state->late_entry_mi_fragment[slot][3][1] << 12) |
            (state->late_entry_mi_fragment[slot][1][2] << 8)  | (state->late_entry_mi_fragment[slot][2][2] << 4)  | (state->late_entry_mi_fragment[slot][3][2] << 0);

  go_test = (state->late_entry_mi_fragment[slot][4][0] << 32L) | (state->late_entry_mi_fragment[slot][5][0] << 28) | (state->late_entry_mi_fragment[slot][6][0] << 24) |
            (state->late_entry_mi_fragment[slot][4][1] << 20) | (state->late_entry_mi_fragment[slot][5][1] << 16) | (state->late_entry_mi_fragment[slot][6][1] << 12) |
            (state->late_entry_mi_fragment[slot][4][2] << 8)  | (state->late_entry_mi_fragment[slot][5][2] << 4)  | (state->late_entry_mi_fragment[slot][6][2] << 0);

  for (j = 0; j < 3; j++)
  {
    for (i = 0; i < 12; i++)
    {
      mi_go_bits[i] = (( mi_test << (i+j*12) ) & 0x800000000) >> 35; 
      mi_go_bits[i+12] = (( go_test << (i+j*12) ) & 0x800000000) >> 35; 
    }
    //execute golay decode and assign pass or fail to g
    if ( Golay_24_12_decode(mi_go_bits) ) g[j] = 1;
    else g[j] = 0;

    for (i = 0; i < 12; i++)
    {
      mi_corrected = mi_corrected << 1;
      mi_corrected |= mi_go_bits[i];
      go_corrected = go_corrected << 1;
      go_corrected |= mi_go_bits[i+12];
    }
  }

  int mi_final = 0; 
  mi_final = (mi_corrected >> 4) & 0xFFFFFFFF;

  if (g[0] && g[1] && g[2])
  {
    if (slot == 0 && state->payload_algid != 0)
    {
      if (state->payload_mi != mi_final) 
      {
        fprintf (stderr, "%s", KCYN);
        fprintf (stderr, " Slot 1 PI/LFSR and Late Entry MI Mismatch - %08X : %08X \n", state->payload_mi, mi_final);
        state->payload_mi = mi_final; 
        fprintf (stderr, "%s", KNRM);
      } 
    }
    if (slot == 1 && state->payload_algidR != 0)
    {
      if (state->payload_miR != mi_final)
      {
        fprintf (stderr, "%s", KCYN);
        fprintf (stderr, " Slot 2 PI/LFSR and Late Entry MI Mismatch - %08X : %08X \n", state->payload_miR, mi_final);
        state->payload_miR = mi_final;
        fprintf (stderr, "%s", KNRM);
      } 
    }

  }

}

void dmr_alg_refresh (dsd_opts * opts, dsd_state * state)
{
  if (state->currentslot == 0)
  {
    state->dropL = 256;

    if (state->K1 != 0) 
    {
      state->DMRvcL = 0;
    }

    if (state->payload_algid >= 0x21)
    {
      LFSR (state);
      fprintf (stderr, "\n");
    }
  }
  if (state->currentslot == 1)
  {
    state->dropR = 256;

    if (state->K1 != 0) 
    {
      state->DMRvcR = 0;
    }

    if (state->payload_algidR >= 0x21)
    {
      LFSR (state);
      fprintf (stderr, "\n");
    }
  }

}

void dmr_alg_reset (dsd_opts * opts, dsd_state * state)
{
  state->dropL = 256;
  state->dropR = 256;
  state->DMRvcL = 0;
  state->DMRvcR = 0; 
  state->payload_miP = 0; 
}

//handle Single Burst (Voice Burst F) or Reverse Channel Signalling 
//(Embedded or Dedicated Outbound, not Stand Alone (MS sourced) or Direct Mode)
//MS Sourced or Direct Mode probably would be handled poorly by the demodulator
void dmr_sbrc (dsd_opts * opts, dsd_state * state, uint8_t power)
{
  int i;
  uint8_t slot = state->currentslot;
  uint8_t sbrc_interleaved[32];
  uint8_t sbrc_return[32];
  memset (sbrc_interleaved, 0, sizeof(sbrc_interleaved));
  memset (sbrc_return, 0, sizeof(sbrc_return));
  uint32_t irr_err = 0;
  uint32_t sbrc_hex = 0;
  uint16_t crc_extracted = 7777;
  uint16_t crc_computed = 9999;
  uint8_t crc_okay = 0;

  // 9.3.2 Pre-emption and power control Indicator (PI)
  // 0 - The embedded signalling carries information associated to the same logical channel or the Null embedded message
  // 1 - The embedded signalling carries RC information associated to the other logical channel

  for(i = 0; i < 32; i++) sbrc_interleaved[i] = state->dmr_embedded_signalling[slot][5][i + 8];
  //power == 0 should be single burst
  if (power == 0) irr_err = BPTC_16x2_Extract_Data(sbrc_interleaved, sbrc_return, 0);
  //power == 1 should be reverse channel -- still need to check the interleave inside of BPTC
  if (power == 1) irr_err = BPTC_16x2_Extract_Data(sbrc_interleaved, sbrc_return, 1);
  //bad emb burst, never set a valid power indicator value (probably 9)
  if (power > 1) goto SBRC_END; 

  //RC Channel CRC 7 Mask = 0x7A; CRC bits are used as privacy indicators on 
  //Single Voice Burst F (see below), other moto values seem to exist there as well
  //unknown what the other values are (see Cap+ 0x313)
  if (power == 1) //RC
  {
    crc_extracted = 0;
    for (i = 0; i < 7; i++)
    {
      crc_extracted = crc_extracted << 1;
      crc_extracted = crc_extracted | sbrc_return[i+4];
    }
    crc_extracted = crc_extracted ^ 0x7A;
    crc_computed = crc7((uint8_t *) sbrc_return, 11);
    if (crc_extracted == crc_computed) crc_okay = 1;
    if (opts->payload == 1) fprintf (stderr, " CRC EXT %02X, CRC CMP %02X", crc_extracted, crc_computed);
  }
  else crc_okay = 1; //SB
  
  fprintf (stderr, "\n %s", KCYN);
  if (power == 0) fprintf (stderr, " SB: ");
  if (power == 1) fprintf (stderr, " RC: ");
  for(i = 0; i < 11; i++)
  {
    sbrc_hex = sbrc_hex << 1;
    sbrc_hex |= sbrc_return[i] & 1; //sbrc_return
    fprintf (stderr, "%d", sbrc_return[i]);
  }
  fprintf (stderr, " - %03X", sbrc_hex);
  fprintf (stderr, "%s", KNRM);

  if (crc_okay == 0)
  {
    fprintf (stderr, "%s", KRED);
    fprintf (stderr, " (CRC ERR)");
    fprintf (stderr, "%s", KNRM);
  }

  //sbrc_hex value of 0x313 seems to be some Cap+ Thing, 
  //also observed 0x643 on another cap+ system (site id, status? something?)
  //I've observed it in the Private Cap+ TXI calls as well
  //there also seems to be a correlation between the SVC bits for TXI (reserved=3) and these extra cap+ values
  uint8_t sbrc_opcode = sbrc_hex; 
  uint8_t alg = sbrc_hex & 3;
  uint16_t key = (sbrc_hex >> 3) & 0xFF;

  if (irr_err != 0) fprintf (stderr, "%s (FEC ERR) %d %s", KRED, irr_err, KNRM);
  if (irr_err == 0)
  {
    if (sbrc_hex == 0) ; //NULL
    else if (sbrc_hex == 0x313)
    {
      //Cap+ Thing? Observed On Cap+ Systems
      // fprintf (stderr, " Cap+ Thing?");
    } 
    else
    {

      if (slot == 0)
      {
        //key and alg only present SOME times, not all,
        //also, intermixed with other signalling
        //needs more study first!
        if (state->dmr_so & 0x40 && key != 0 && state->payload_keyid == 0)
        {
          if (opts->payload == 1)
          {
            fprintf (stderr, "%s ", KYEL);
            fprintf (stderr, "\n Slot 1");
            fprintf (stderr, " DMR LE SB ALG ID: %X KEY ID: %0X", alg + 0x20, key);
            fprintf (stderr, "%s ", KNRM);
          }
          
          //needs more study before assignment
          //state->payload_keyid = key;
          //state->payload_algid = alg + 0x20; //assuming DMRA approved alg values (moto patent)
        }
      }
      if (slot == 1)
      {
        if (state->dmr_soR & 0x40 && key != 0 && state->payload_keyidR == 0)
        {
          if (opts->payload == 1)
          {
            fprintf (stderr, "%s ", KYEL);
            fprintf (stderr, "\n Slot 2");
            fprintf (stderr, " DMR LE SB ALG ID: %X KEY ID: %0X", alg + 0x20, key);
            fprintf (stderr, "%s ", KNRM);
          }
          
          //needs more study before assignment
          //state->payload_keyidR = key;
          //state->payload_algidR = alg + 0x20; //assuming DMRA approved alg values (moto patent)
        }
      }

    }

  }

  SBRC_END:

  //'DSP' output to file -- only RC, or RC and SB?
  if (power == 1 && opts->use_dsp_output == 1 && sbrc_hex != 0) //if not NULL
  {
    FILE * pFile; //file pointer
    pFile = fopen (opts->dsp_out_file, "a");
    fprintf (pFile, "\n%d 99 ", slot+1); //'99' is RC designation value
    int k = 0;
    for (i = 0; i < 24; i++) //12 bytes, SB or RC
    {
      //check to see if k++ starts at zero, or at 1
      int sbrc_nib = (state->dmr_embedded_signalling[slot][5][k++] << 3) | (state->dmr_embedded_signalling[slot][5][k++] << 2) | (state->dmr_embedded_signalling[slot][5][k++] << 1) | (state->dmr_embedded_signalling[slot][5][k++] << 0);
      fprintf (pFile, "%X", sbrc_nib);
    }
    fclose (pFile);
  } 

}

