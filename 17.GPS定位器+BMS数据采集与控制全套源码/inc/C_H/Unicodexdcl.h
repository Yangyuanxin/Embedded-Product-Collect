/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*******************************************************************************
 * Filename:
 * ---------
 *  Unicodexdcl.h
 *
 * Project:
 * --------
 *  MAUI
 *
 * Description:
 * ------------
 *  
 *
 * Author:
 * -------
 *  
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/
 
/**
 * Copyright Notice
 * ?2002 - 2003, Pixtel Communications, Inc., 1489 43rd Ave. W.,
 * Vancouver, B.C. V6M 4K8 Canada. All Rights Reserved.
 *  (It is illegal to remove this copyright notice from this software or any
 *  portion of it)
 */

/************************************************************** 
 FILENAME   : Unicodexdcl.h 
 PURPOSE : Gives the current build settings. 
 REMARKS : Manish 
 AUTHOR     : Pixtel Engineers 
 DATE    : . 
 **************************************************************/

#ifndef _UNICODEXDCL_H_
#define _UNICODEXDCL_H_

#include "MMIDataType.h"
#include "kal_non_specific_general_types.h"
/* the application include this header to get related API */
/* DOM-NOT_FOR_SDK-BEGIN */
/*****************************************************************************
 * FUNCTION
 *  mmi_wc_to_ucs2
 * DESCRIPTION
 *  convert unicode to UCS2 encoding
 * PARAMETERS
 *  unicode       :  [IN]        Value to be encoded
 *  charLength    :  [OUT]       The length of the character
 *  arrOut        :  [OUT]       The output character
 * RETURNS
 *  U8 -> Status, 0 is failed, 1 is success.
 *****************************************************************************/
extern U8 mmi_wc_to_ucs2(U16 unicode, U8 *charLength, U8 *arrOut);

/*****************************************************************************
 * FUNCTION
 *  mmi_ucs2_to_wc
 * DESCRIPTION
 *  convert UCS2 encoded scheme to unicode
 * PARAMETERS
 *  pUnicode      :  [IN]        array containing  UCS2 encoded characters
 *  arr           :  [IN]        unicode equivalent
 * RETURNS
 *  U8 -> Status, 0 is failed, 1 is success.
 *****************************************************************************/
extern U8 mmi_ucs2_to_wc(PU16 pUnicode, PU8 arrOut);

/*****************************************************************************
 * FUNCTION
 *  mmi_ucs2strlen
 * DESCRIPTION
 *  Gives the length of UCS2 encoded string
 * PARAMETERS
 *  arrOut     : [IN]        array containing  UCS2 encoded characters
 * RETURNS
 *  S32-> Status
 *****************************************************************************/
extern S32 mmi_ucs2strlen(const CHAR *arrOut);

/*****************************************************************************
 * FUNCTION
 *  mmi_ucs2cmp
 * DESCRIPTION
 *  compares two strings
 * PARAMETERS
 *  string1    : [IN]        String1
 *  string2    : [OUT]       String2
 * RETURNS
 *  <0 - string1 less than string2
 *   0 - string1 identical to string2
 *  >0 - string1 greater than string2
 *****************************************************************************/
extern S32 mmi_ucs2cmp(const CHAR *string1, const CHAR *string2);

/*****************************************************************************
 * FUNCTION
 *  mmi_ucs2cpy
 * DESCRIPTION
 *  copies the one UCS2 encoded string to other
 * PARAMETERS
 *  strDestination     : [OUT]       Destination string
 *  strSource          : [IN]        source string
 * RETURNS
 *  CHAR * -> pointer to destination string or NULL
 *****************************************************************************/
extern CHAR *mmi_ucs2cpy(CHAR *strDestination, const CHAR *strSource);

/*****************************************************************************
 * FUNCTION
 *  mmi_ucs2ncmp
 * DESCRIPTION
 *  The function is used for compare UCS2 characters of two strings. Size is 
 *  the character number not the byte numbers.
 * PARAMETERS
 *  string1    : [IN]  UCS2 string1
 *  string2    : [IN]  UCS2 string2
 *  count      : [IN]  Number in character (not byte)  
 * RETURNS
 *  <0 - string1 less than string2
 *   0 - string1 identical to string2
 *  >0 - string1 greater than string2
 *****************************************************************************/
extern S32 mmi_ucs2ncmp(const CHAR *string1, const CHAR *string2, U32 count);

/*****************************************************************************
 * FUNCTION
 *  mmi_ucs2ncpy
 * DESCRIPTION
 *  The function is used for copy UCS2 characters of one string to another. 
 *  If size is less than the length of strSource, a null character is still 
 *  appended automatically to strDestination. If size is greater than the 
 *  length of strSource, strDestination is padding with null characters up 
 *  to length size. The function behavior is a little bit different from strncpy.
 * PARAMETERS
 *  strDestination    :  [OUT]       StrDest-> Destination string
 *  strSource         :  [IN]        Source string
 *  size              :  [IN]        The size that needs to copy.
 * RETURNS
 *  CHAR * -> pointer to destination string or NULL
 *****************************************************************************/
extern CHAR *mmi_ucs2ncpy(CHAR *strDestination, const CHAR *strSource, U32 size);

/*****************************************************************************
 * FUNCTION
 *  mmi_ucs2cat
 * DESCRIPTION
 *  The function is used for append strSource to strDestination and terminate
 *  the resulting string with a null character. 
 * PARAMETERS
 *  strDestination     : [OUT]     Destination string     
 *  strSource          : [IN]      Source string 
 * RETURNS
 *  Return the destination string
 *****************************************************************************/
extern CHAR *mmi_ucs2cat(CHAR *strDestination, const CHAR *strSource);

/*****************************************************************************
 * FUNCTION
 *  mmi_ucs2ncat
 * DESCRIPTION
 *  The function is used for append the first size characters of strSource 
 *  to strDestination and terminate the resulting string with a null character. 
 * PARAMETERS
 *  strDestination    :  [OUT]   UCS2 destination string      
 *  strSource         :  [IN]    UCS2 source string    
 *  size              :  [IN]    Size in character (not byte)          
 * RETURNS
 *  Return the destination string
 *****************************************************************************/
extern CHAR *mmi_ucs2ncat(CHAR *strDestination, const CHAR *strSource, U32 size);


/*****************************************************************************
 * FUNCTION
 *  mmi_ucs2trim_left
 * DESCRIPTION
 *  Skip over the TrimChars from the begin character.
 * PARAMETERS
 *  strSrc        :  [IN]   The source string.     
 *  TrimChars     :  [IN]   The trim chars.     
 * RETURNS
 *  Return the result string.
 *****************************************************************************/
extern CHAR *mmi_ucs2trim_left(const CHAR *strSrc, const CHAR *TrimChars);

/*****************************************************************************
 * FUNCTION
 *  mmi_ucs2find
 * DESCRIPTION
 *  Search for the break_chars and store how many chars have been read over to reach
 *  the desired break_char in read_length.
 * PARAMETERS
 *  strSrc        :  [IN]    The source string.    
 *  break_chars   :  [IN]    The break_chars.    
 *  read_length   :  [OUT]   The number of chars to reach the break_char.     
 *  direction     :  [IN]    1: sesarch forward ; -1: search backward.
 * RETURNS
 *  1: found the break_chars ;  -1: searched over the whole strSrc and do not fing the break_chars
 *****************************************************************************/
extern S32 mmi_ucs2find(const CHAR *strSrc, const CHAR *break_chars, S32 *read_length, S16 direction);

/*****************************************************************************
 * FUNCTION
 *  mmi_ucs2toi
 * DESCRIPTION
 *  to parse over a int number, including positive and negative signs (+,-).
 * PARAMETERS
 *  strSrc       :   [IN]    The source string.    
 *  out_num      :   [OUT]   The ourput number.
 *  read_length  :   [OUT]   The read string length.      
 * RETURNS
 *  1: parse successfully ; -1: parse number fail.
 *****************************************************************************/
extern S32 mmi_ucs2toi(const CHAR *strSrc, S32 *out_num, S32 *read_length);

/*****************************************************************************
 * FUNCTION
 *  mmi_asc_to_ucs2
 * DESCRIPTION
 *  The function is used for convert ANSII string to UCS2 string. The caller
 *  need to make sure the pOutBuffer  size must have enough space or the function
 *  causes the memory corruption. The function will add the terminated character
 *  at the end of pOutBuffer array. The byte order of UCS2 character(output param)
 *  is little endian.
 * PARAMETERS
 *  pOutBuffer    :  [OUT]    The output buffer. 
 *  pInBuffer     :  [IN]     The input buffer.
 * RETURNS
 *  Return the bytes to convert.
 *****************************************************************************/
extern U16 mmi_asc_to_ucs2(CHAR *pOutBuffer, CHAR *pInBuffer);

/*****************************************************************************
 * FUNCTION
 *  mmi_asc_n_to_ucs2
 * DESCRIPTION
 *  The function is used for convert the characters of ANSII string to UCS2
 *  string. The caller need to make sure the pOutBuffer size is greater than 
 *  len and the function doesn't add the terminated character at the end of 
 *  the pOutBuffer array. The byte order of UCS2 character(output param) is
 *  little endian.
 * PARAMETERS
 *  pOutBuffer    :  [OUT]     UCS2 destination string.     
 *  pInBuffer     :  [IN]      ANSII source string        
 *  len           :  [IN]      Length in bytes     
 * RETURNS
 *  Return the bytes to convert.
 *****************************************************************************/
extern U16 mmi_asc_n_to_ucs2(CHAR *pOutBuffer, CHAR *pInBuffer, U32 len);

/*****************************************************************************
 * FUNCTION
 *  mmi_asc_to_ucs2_ex
 * DESCRIPTION
 *  The function is used for convert ANSII string to UCS2 string. The caller
 *  need to make sure the pOutBuffer  size must have enough space or the function
 *  causes the memory corruption. The function will add the terminated character
 *  at the end of pOutBuffer array. The byte order of UCS2 character(output param)
 *  is little endian.
 *  Unlike mmi_asc_to_ucs2, it will ignore the controls in ANSII string. 
 * PARAMETERS
 *  pOutBuffer     : [OUT]   UCS2 destination string  
 *  pInBuffer      : [IN]    ANSII source string  
 * RETURNS
 *  Return the bytes to convert.
 *****************************************************************************/
extern U16 mmi_asc_to_ucs2_ex(CHAR *pOutBuffer, CHAR *pInBuffer);

/*****************************************************************************
 * FUNCTION
 *  mmi_asc_n_to_ucs2_Ex
 * DESCRIPTION
 *  The function is used for convert the characters of ANSII string to UCS2
 *  string. The caller need to make sure the pOutBuffer size is greater than 
 *  len and the function doesn't add the terminated character at the end of 
 *  the pOutBuffer array. The byte order of UCS2 character(output param) is
 *  little endian.
 *  Unlike mmi_asc_n_to_ucs2, it will ignore the controls in ANSII string.
 * PARAMETERS
 *  pOutBuffer   :   [OUT]  UCS2 destination string.        
 *  pInBuffer    :   [IN]   ANSII source string      
 *  len          :   [IN]   size in bytes     
 * RETURNS
 *  Return the bytes to convert.
 *****************************************************************************/
extern U16 mmi_asc_n_to_ucs2_ex(CHAR *pOutBuffer, CHAR *pInBuffer, U32 len);

/*****************************************************************************
 * FUNCTION
 *  mmi_ucs2_to_asc
 * DESCRIPTION
 *  The function is used for convert UCS2 string to ANSII string. 
 *  The caller need to make sure the pOutBuffer  size must have enough space 
 *  or the function causes the memory corruption. The function will add the 
 *  terminated character at the end of pOutBuffer array. The byte order of 
 *  UCS2 character(input param) is little endian.
 * PARAMETERS
 *  pOutBuffer    :  [OUT]  The output Buffer. 
 *  pInBuffer     :  [IN]   The input Buffer  
 * RETURNS
 *  U16
 *****************************************************************************/
extern U16 mmi_ucs2_to_asc(CHAR *pOutBuffer, CHAR *pInBuffer);

/*****************************************************************************
 * FUNCTION
 *  mmi_ucs2_n_to_asc
 * DESCRIPTION
 *  The function is used for convert the characters of UCS2 string to ANSII
 *  string. The caller need to make sure the pOutBuffer size is greater than 
 *  len and the function doesn't add the terminated character at the end of 
 *  the pOutBuffer array. The byte order of UCS2 character(input param) is little endian.
 * PARAMETERS
 *  pOutBuffer    :  [OUT]   ANSII destination string.       
 *  pInBuffer     :  [IN]    UCS2 source string.         
 *  len           :  [IN]    Length in bytes.      
 * RETURNS
 *  U16
 *****************************************************************************/
extern U16 mmi_ucs2_n_to_asc(CHAR *pOutBuffer, CHAR *pInBuffer, U32 len);

/*****************************************************************************
 * FUNCTION
 *  mmi_ucs2str
 * DESCRIPTION
 *  Finds the first occurrence of string2 in string1
 * PARAMETERS
 *  str1       : [IN]  string to search in.       
 *  str2       : [IN]  string to search for.      
 * RETURNS
 *  returns a pointer to the first occurrence of string2 in
 *  string1, or NULL if string2 does not occur in string1
 *****************************************************************************/
extern CHAR *mmi_ucs2str(const CHAR *str1, const CHAR *str2);

/*****************************************************************************
 * FUNCTION
 *  mmi_ucs2chr
 * DESCRIPTION
 *  Searches a UCS2 encoded string for a given wide-character,
 *  which may be the null character L'\0'.
 * PARAMETERS
 *  strSrc   :     [IN]  UCS2 encoded string to search in.       
 *  c        :     [IN]  UCS2 encoded wide-character to search for.      
 * RETURNS
 *  returns pointer to the first occurrence of ch in string
 *  returns NULL if ch does not occur in string
 *****************************************************************************/
extern CHAR *mmi_ucs2chr(const CHAR *string,  WCHAR ch);

/*****************************************************************************
 * FUNCTION
 *  mmi_ucs2rchr
 * DESCRIPTION
 *  Scan a UCS2 encoded string for the last occurrence of a character.
 * PARAMETERS
 *  string     :   [IN]  UCS2 encoded string to search in.       
 *  ch         :   [IN]  UCS2 encoded wide-character to search for.      
 * RETURNS
 *  returns pointer to the last occurrence of ch in string
 *  returns NULL if ch does not occur in string
 *****************************************************************************/
extern CHAR *mmi_ucs2rchr(const CHAR * string, WCHAR ch);

/*****************************************************************************
 * FUNCTION
 *  mmi_ucs2upr
 * DESCRIPTION
 *  mmi_ucs2upr converts upper-case characters in a null-terminated 
 *  UCS2 encoded string their upper-case equivalents.  
 * PARAMETERS
 *  string   : [IN]  UCS2 encoded string to change to upper case.      
 * RETURNS
 *  returns a pointer to the converted string. Because the modification is 
 *  done in place, the pointer returned is the same as the pointer passed 
 *  as the input argument.
 *****************************************************************************/
extern CHAR *mmi_ucs2upr(CHAR *string);

/*****************************************************************************
 * FUNCTION
 *  mmi_ucs2lwr
 * DESCRIPTION
 *  mmi_ucs2lwr converts lower-case characters in a null-terminated 
 *  UCS2 encoded string their lower-case equivalents.  
 * PARAMETERS
 *  string   : [IN]  UCS2 encoded string to change to upper case.      
 * RETURNS
 *  returns a pointer to the converted string. Because the modification is 
 *  done in place, the pointer returned is the same as the pointer passed 
 *  as the input argument.
 *****************************************************************************/
extern CHAR *mmi_ucs2lwr(CHAR *string);

/*****************************************************************************
 * FUNCTION
 *  mmi_ucs2icmp
 * DESCRIPTION
 *  mmi_ucs2icmp perform a case-insensitive UCS2 encoded string
 *  (wide-character) comparison.
 * PARAMETERS
 *  str_src  : [IN]  UCS2 encoded destination string for 
 *                  left-hand side of comparison.
 *  str_dst  : [IN]  UCS2 encoded source string for right-hand 
 *                  side of comparison. 
 * RETURNS
 *  returns -1 if str_src <  str_dst
 *  returns  0 if str_src == str_dst
 *  returns +1 if str_src >  str_dst
 *****************************************************************************/
extern S32 mmi_ucs2icmp(const CHAR *str_src, const CHAR *str_dst);

/*****************************************************************************
 * FUNCTION
 *  mmi_ucs2nicmp
 * DESCRIPTION
 *  Compares two UCS2 encoded strings for lexical order without regard to case. 
 *  The comparison stops after: 
 *  (1) a difference between the strings is found; 
 *  (2) the end of the strings is reached;
 *  (3) count characters have been compared.
 * PARAMETERS
 *  str_src  : [IN]  UCS2 encoded destination string for 
 *                  left-hand side of comparison.
 *  str_dst  : [IN]  UCS2 encoded source string for right-hand 
 *                  side of comparison. 
 *  count    : [IN]  Number of characters to compare.    
 * RETURNS
 *  returns -1 if str_src <  str_dst
 *  returns  0 if str_src == str_dst
 *  returns +1 if str_src >  str_dst
 *****************************************************************************/
extern S32 mmi_ucs2nicmp(const CHAR *str_src, const CHAR *str_dst, U32 count);

/*****************************************************************************
 * FUNCTION
 *  mmi_tolower
 * DESCRIPTION
 *  Convert character to lowercase.
 * PARAMETERS
 *  c   :  [IN]    Character to convert. 
 * RETURNS
 *  return the point to the converted character.
 *****************************************************************************/
extern CHAR* mmi_tolower(CHAR *c);

/*****************************************************************************
 * FUNCTION
 *  mmi_toupper
 * DESCRIPTION
 *  Convert character to uppercase.
 * PARAMETERS
 *  c    : [IN]     Character to convert. 
 * RETURNS
 *  return the point to the converted character.
 *****************************************************************************/
extern CHAR* mmi_toupper(CHAR *c);
/* DOM-NOT_FOR_SDK-END */

/*****************************************************************************
 * FUNCTION
 *  mmi_asc_to_wcs
 * DESCRIPTION
 *  The function is used for convert ANSII string to UCS2 string. The caller
 *  need to make sure the pOutBuffer  size must have enough space or the function
 *  causes the memory corruption. The function will add the terminated character
 *  at the end of pOutBuffer array. The byte order of UCS2 character(output param)
 *  is little endian.
 * PARAMETERS
 *  pOutBuffer    :  [OUT]  UCS2 destination string   
 *  pInBuffer     :  [IN]   ANSII source string  
 * RETURNS
 *  Return the bytes to convert.
 *****************************************************************************/
extern U16 mmi_asc_to_wcs(WCHAR *pOutBuffer, CHAR *pInBuffer);

/*****************************************************************************
 * FUNCTION
 *  mmi_asc_n_to_wcs
 * DESCRIPTION
 *  The function is used for convert the characters of ANSII string to UCS2
 *  string. The caller need to make sure the pOutBuffer size is greater than 
 *  len and the function doesn't add the terminated character at the end of 
 *  the pOutBuffer array. The byte order of UCS2 character(output param) is
 *  little endian.
 * PARAMETERS
 *  pOutBuffer     : [OUT]   UCS2 destination string.      
 *  pInBuffer      : [IN]    ANSII source string     
 *  len            : [IN]    size in bytes   
 * RETURNS
 *  Return the bytes to convert.
 *****************************************************************************/
extern U16 mmi_asc_n_to_wcs(WCHAR *pOutBuffer, CHAR *pInBuffer, U32 len);

/*****************************************************************************
 * FUNCTION
 *  mmi_wcs_to_asc
 * DESCRIPTION
 *  The function is used for convert UCS2 string to ANSII string. 
 *  The caller need to make sure the pOutBuffer  size must have enough space 
 *  or the function causes the memory corruption. The function will add the 
 *  terminated character at the end of pOutBuffer array. 
 * PARAMETERS
 *  pOutBuffer    :  [OUT]   ANSII destination string  
 *  pInBuffer     :  [IN]    UCS2 source string 
 * RETURNS
 *  Return the bytes to convert.
 *****************************************************************************/
extern U16 mmi_wcs_to_asc(CHAR *pOutBuffer, WCHAR *pInBuffer);

/*****************************************************************************
 * FUNCTION
 *  mmi_wcs_n_to_asc
 * DESCRIPTION
 *  The function is used for convert the characters of UCS2 string to ANSII
 *  string. The caller need to make sure the pOutBuffer size is greater than 
 *  len and the function doesn't add the terminated character at the end of 
 *  the pOutBuffer array. The byte order of UCS2 character(input param) is 
 *  little endian.
 * PARAMETERS
 *  pOutBuffer     : [OUT]    ANSII destination string.     
 *  pInBuffer      : [IN]     UCS2 source string.    
 *  len            : [IN]     Length in bytes.  
 * RETURNS
 *  Return the bytes to convert.
 *****************************************************************************/
extern U16 mmi_wcs_n_to_asc(CHAR *pOutBuffer, WCHAR *pInBuffer, U32 len);

/* DOM-NOT_FOR_SDK-BEGIN */
/*****************************************************************************
 * FUNCTION
 *  mmi_asc_to_wcs_ex
 * DESCRIPTION
 *  The function is used for convert ANSII string to UCS2 string. The caller
 *  need to make sure the dest size must have enough space or the function
 *  causes the memory corruption. The function will add the terminated character
 *  at the end of dest array. The byte order of UCS2 character(output param)
 *  is little endian.
 * PARAMETERS
 *  dest           : [OUT]  UCS2 destination string.   
 *  src            : [IN]   ANSII source string.
 *  src_end_pos    : [OUT]  The point to the end of converted sub-string at the input string.
 * RETURNS
 *  Return the bytes to convert.
 *****************************************************************************/
extern U16 mmi_asc_to_wcs_ex(WCHAR *dest, CHAR *src, U32 *src_end_pos);

/*****************************************************************************
 * FUNCTION
 *  mmi_asc_n_to_wcs_ex
 * DESCRIPTION
 *  The function is used for convert the characters of ANSII string to UCS2
 *  string. The caller need to make sure the dest size must have enough space 
 *  or the function causes the memory corruption. The function will add the 
 *  terminated character at the end of dest array. The byte order of UCS2 
 *  character(output param) is little endian.
 * PARAMETERS
 *  dest          :  [OUT]  UCS2 destination string.   
 *  src           :  [IN]   ANSII source string.
 *  len           :  [IN]   Size in bytes 
 *  src_end_pos   :  [OUT]  The point to the end of converted sub-string at the input string.
 * RETURNS
 *  Return the bytes to convert.
 *****************************************************************************/\
extern U16 mmi_asc_n_to_wcs_ex(WCHAR *dest, CHAR *src, U32 len, U32 *src_end_pos);
/* DOM-NOT_FOR_SDK-END */

/*****************************************************************************
 * FUNCTION
 *  mmi_wcslen
 * DESCRIPTION
 *  Gets the number of characters of the given UCS2 encoded string(wide-character), 
 *  not including the final null wide-characters.
 * PARAMETERS          
 *  string   : [IN]  UCS2 encoded string(wide-character) which length is to
 *                   be computed.      
 * RETURNS
 *  The number of characters of a wide-character string.
 *****************************************************************************/
extern S32 mmi_wcslen(const WCHAR *string);

/*****************************************************************************
 * FUNCTION
 *  mmi_wcscmp
 * DESCRIPTION
 *  Compares two UCS2 encoded strings(wide-character) and returns an integer to 
 *  indicate whether the destination string is less than the source string, 
 *  the two are equal, or whether the destination string is greater than the 
 *  source string.
 * PARAMETERS          
 *  str_src  : [IN]  UCS2 encoded destination string(wide-character) for 
 *                  left-hand side of comparison.
 *  str_dst  : [IN]  UCS2 encoded source string(wide-character) for right-hand 
 *                  side of comparison.       
 * RETURNS
 *  returns <0 if str_src <  str_dst
 *  returns  0 if str_src == str_dst
 *  returns >0 if str_src >  str_dst
 *****************************************************************************/
extern S32 mmi_wcscmp(const WCHAR *str_src, const WCHAR *str_dst);

/*****************************************************************************
 * FUNCTION
 *  mmi_wcsncmp
 * DESCRIPTION
 *  Compares two UCS2 encoded strings(wide-character) for lexical order. 
 *  The comparison stops after: 
 *  (1) a difference between the strings is found; 
 *  (2) the end of the strings is reached;
 *  (3) count characters have been compared (wide-character strings).
 * PARAMETERS
 *  str_src  : [IN]  UCS2 encoded destination string(wide-character) for 
 *                  left-hand side of comparison.
 *  str_dst  : [IN]  UCS2 encoded source string(wide-character) for right-hand 
 *                  side of comparison. 
 *  count    : [IN]  Number of characters to compare.    
 * RETURNS
 *  returns -1 if str_src <  str_dst
 *  returns  0 if str_src == str_dst
 *  returns +1 if str_src >  str_dst
 *****************************************************************************/
extern S32 mmi_wcsncmp(const WCHAR *str_src, const WCHAR *str_dst, U32 count);

/*****************************************************************************
 * FUNCTION
 *  mmi_wcscpy
 * DESCRIPTION
 *  Copies the UCS2 encoded source string(wide-character) into the destination 
 *  string; Assumes enough space in the destination string.
 * PARAMETERS
 *  str_dst  : [OUT]  UCS2 encoded destination string(wide-character) over which 
 *                   the source string is to be copied.
 *  str_src  : [IN]   UCS2 encoded source string(wide-character) to be copied 
 *                   over the destination string.
 * RETURNS
 *  A pointer to the destination string.
 *****************************************************************************/
extern WCHAR *mmi_wcscpy(WCHAR *str_dst, const WCHAR *str_src);

/*****************************************************************************
 * FUNCTION
 *  mmi_wcsncpy
 * DESCRIPTION
 *  Copies count characters from the source string to the destination string.  
 *  Note:
 *  Unlike ANSI C standard library function wcsncpy. If count is less than the 
 *  length of source, NULL wide-characters also is put onto the end of the 
 *  copied string. But if count is greater than the length of sources, str_dst 
 *  is not padded with null characters to length count (wide-characters).
 * PARAMETERS
 *  str_dst  : [OUT]  UCS2 encoded destination string(wide-character) over which 
 *                   the source string is to be copied.
 *  str_src  : [IN]   UCS2 encoded source string(wide-character) to be copied 
 *                   over the destination string.
 *  count    : [IN]   max number of characters to copy.
 * RETURNS
 *  A pointer to the destination string.
 *****************************************************************************/
extern WCHAR *mmi_wcsncpy(WCHAR *str_dst, const WCHAR *str_src, U32 count);

/*****************************************************************************
 * FUNCTION
 *  mmi_wcscat
 * DESCRIPTION
 *  Concatenates the source string onto the end of the destination string. 
 *  Assumes enough space in the destination string.
 * PARAMETERS
 *  str_dst  : [OUT]  UCS2 encoded destination string(wide-character) over which 
 *                   "str_src" is to be copied.      
 *  str_src  : [IN]   UCS2 encoded source string(wide-character) to be copied 
 *                   over "str_dst".      
 * RETURNS
 *  A pointer to the destination string.
 *****************************************************************************/
extern WCHAR *mmi_wcscat(WCHAR *str_dst, const WCHAR *str_src);

/*****************************************************************************
 * FUNCTION
 *  mmi_wcsncat
 * DESCRIPTION
 *  Appends at most count characters of the source string onto the
 *  end of destination string, and ALWAYS terminates with a null character.
 *  If count is greater than the length of source string, the length of source 
 *  string is used instead.  (Like app_ucs2_wcsncpy, this routine does not pad out
 *  to count characters).
 * PARAMETERS
 *  str_dst  : [OUT]  UCS2 encoded destination string(wide-character) to be appended.        
 *  str_src  : [IN]   UCS2 encoded source string(wide-character) to be appended 
 *                   to the end of destination string.      
 *  count    : [IN]   Number of characters to append.       
 * RETURNS
 *  A pointer to the destination string.
 *****************************************************************************/
extern WCHAR *mmi_wcsncat(WCHAR *str_dst, const WCHAR *str_src, U32 count);

/*****************************************************************************
 * FUNCTION
 *  mmi_wcsstr
 * DESCRIPTION
 *  Find a substring.
 * PARAMETERS
 *  string       : [IN]  UCS2 encoded string(wide-character) to search.       
 *  str_char_set : [IN]  UCS2 encoded string(wide-character) to search for.      
 * RETURNS
 *  Returns a pointer to the first occurrence of str_char_set in string, or 
 *  NULL if str_char_set does not appear in string. If str_char_set points to 
 *  a string of zero length, the function returns string. 
 *****************************************************************************/
extern WCHAR *mmi_wcsstr(WCHAR *string, const WCHAR *str_char_set);

/*****************************************************************************
 * FUNCTION
 *  mmi_wcschr
 * DESCRIPTION
 *  Searches a UCS2 encoded string(wide-character) for a given wide-character,
 *  which may be the null character L'\0'.
 * PARAMETERS
 *  string      :  [IN]  UCS2 encoded string(wide-character) to search in.       
 *  ch          :  [IN]  UCS2 encoded wide-character to search for.      
 * RETURNS
 *  returns pointer to the first occurrence of ch in string
 *  returns NULL if ch does not occur in string
 *****************************************************************************/
extern WCHAR *mmi_wcschr(const WCHAR *string,  WCHAR ch);

/*****************************************************************************
 * FUNCTION
 *  mmi_wcsrchr
 * DESCRIPTION
 *  Scan a UCS2 encoded string(wide-character) for the last occurrence of a 
 *  character.
 * PARAMETERS
 *  string      :  [IN]  UCS2 encoded string(wide-character) to search in.       
 *  ch          :  [IN]  UCS2 encoded wide-character to search for.      
 * RETURNS
 *  returns pointer to the last occurrence of ch in string
 *  returns NULL if ch does not occur in string
 *****************************************************************************/
extern WCHAR *mmi_wcsrchr(const WCHAR * string, WCHAR ch);

/*****************************************************************************
 * FUNCTION
 *  mmi_wcslwr
 * DESCRIPTION
 *  mmi_wcslwr converts upper-case characters in a null-terminated 
 *  UCS2 encoded string(wide-character) to their lower-case equivalents.  
 * PARAMETERS
 *  string  :  [IN]  UCS2 encoded string(wide-character) to change to lower 
 *                  case.      
 * RETURNS
 *  returns a pointer to the converted string. Because the modification is 
 *  done in place, the pointer returned is the same as the pointer passed 
 *  as the input argument.
 *****************************************************************************/
extern WCHAR *mmi_wcslwr(WCHAR *string);

/*****************************************************************************
 * FUNCTION
 *  mmi_wcslwr
 * DESCRIPTION
 *  mmi_wcslwr converts upper-case characters in a null-terminated 
 *  UCS2 encoded string(wide-character) to their lower-case equivalents.  
 * PARAMETERS
 *  string  :  [IN]  UCS2 encoded string(wide-character) to change to lower 
 *                  case.      
 * RETURNS
 *  returns a pointer to the converted string. Because the modification is 
 *  done in place, the pointer returned is the same as the pointer passed 
 *  as the input argument.
 *****************************************************************************/
extern WCHAR *mmi_wcsupr(WCHAR *string);

/*****************************************************************************
 * FUNCTION
 *  mmi_wcsicmp
 * DESCRIPTION
 *  mmi_wcsicmp perform a case-insensitive wchar_t UCS2 encoded 
 *  string(wide-character) comparison. 
 * PARAMETERS
 *  str_src  : [IN]  UCS2 encoded destination string(wide-character) for 
 *                  left-hand side of comparison.
 *  str_dst  : [IN]  UCS2 encoded source string(wide-character) for right-hand 
 *                  side of comparison.       
 * RETURNS
 *  returns <0 if str_src <  str_dst
 *  returns  0 if str_src == str_dst
 *  returns >0 if str_src >  str_dst
 *****************************************************************************/
extern S32 mmi_wcsicmp(const WCHAR *str_src, const WCHAR *str_dst);

/*****************************************************************************
 * FUNCTION
 *  mmi_wcsnicmp
 * DESCRIPTION
 *  Compares two UCS2 encoded strings(wide-character) for lexical order 
 *  without regard to case. 
 *  The comparison stops after: 
 *  (1) a difference between the strings is found; 
 *  (2) the end of the strings is reached;
 *  (3) count characters have been compared (wide-character strings).
 * PARAMETERS
 *  str_src  : [IN]  UCS2 encoded destination string(wide-character) for 
 *                  left-hand side of comparison.
 *  str_dst  : [IN]  UCS2 encoded source string(wide-character) for right-hand 
 *                  side of comparison. 
 *  count    : [IN]  Number of characters to compare.    
 * RETURNS
 *  returns -1 if str_src <  str_dst
 *  returns  0 if str_src == str_dst
 *  returns +1 if str_src >  str_dst
 *****************************************************************************/
extern S32 mmi_wcsnicmp(const WCHAR *str_src, const WCHAR *str_dst, U32 count);

/*****************************************************************************
 * FUNCTION
 *  mmi_towlower
 * DESCRIPTION
 *  Convert character to lowercase.
 * PARAMETERS
 *  wc     : [IN]  Character to convert.
 * RETURNS
 *  Each of these routines converts a copy of wc, if possible, and returns the 
 *  result.
 ****************************************************************************/
extern WCHAR mmi_towlower(WCHAR wc);

/*****************************************************************************
 * FUNCTION
 *  mmi_towupper
 * DESCRIPTION
 *  Convert character to uppercase.
 * PARAMETERS
 *  wc     : [IN]  Character to convert.
 * RETURNS
 *  Each of these routines converts a copy of wc, if possible, and returns the 
 *  result.
 *****************************************************************************/
extern WCHAR mmi_towupper(WCHAR wc);

/*****************************************************************************
 * FUNCTION
 *  mmi_wcs_wtoi
 * DESCRIPTION
 *  Convert string  to  integer.
 * PARAMETERS
 *  pInbuf     : [IN] input string
 * RETURNS
 *  returns a pointer to string.
 *****************************************************************************/
extern S32 mmi_wcs_wtoi(WCHAR *pInbuf);

/*****************************************************************************
 * FUNCTION
 *  mmi_wcs_itow
 * DESCRIPTION
 *  Convert an integer to a string.
 * PARAMETERS
 *  value         : [IN]     Number to be converted. 
 *  pOutbuf       : [OUT]    String result. 
 *  radix         : [IN]     Base of value; must be in the range 2 ¨C 36. 
 * RETURNS
 *  returns a pointer to string.
 *****************************************************************************/
extern WCHAR *mmi_wcs_itow(S32 value, WCHAR *pOutbuf, U32 radix);

#endif /* _UNICODEXDCL_H_ */
