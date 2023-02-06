/*
  nuts_bolts.c - Shared functions
  Part of Grbl

  Copyright (c) 2011-2016 Sungeun K. Jeon for Gnea Research LLC
  Copyright (c) 2009-2011 Simen Svale Skogsrud

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "robot.h"

#define MAX_INT_DIGITS 8 // Maximum number of digits in int32 (and float)

// Extracts a floating point value from a string. The following code is based loosely on
// the avr-libc strtod() function by Michael Stumpf and Dmitry Xmelkov and many freely
// available conversion method examples, but has been highly optimized for Grbl. For known
// CNC applications, the typical decimal value is expected to be in the range of E0 to E-4.
// Scientific notation is officially not supported by g-code, and the 'E' character may
// be a g-code word on some CNC systems. So, 'E' notation will not be recognized.
// NOTE: Thanks to Radu-Eosif Mihailescu for identifying the issues with using strtod().
uint8_t read_float(char *line, uint8_t *char_counter, float *float_ptr)
{
  char *ptr = line + *char_counter;
  unsigned char c;

  // Grab first character and increment pointer. No spaces assumed in line.
  c = *ptr++;

  // Capture initial positive/minus character
  bool isnegative = false;
  if (c == '-')
  {
    isnegative = true;
    c = *ptr++;
  }
  else if (c == '+')
  {
    c = *ptr++;
  }

  // Extract number into fast integer. Track decimal in terms of exponent value.
  uint32_t intval = 0;
  int8_t exp = 0;
  uint8_t ndigit = 0;
  bool isdecimal = false;
  while (1)
  {
    c -= '0';
    if (c <= 9)
    {
      ndigit++;
      if (ndigit <= MAX_INT_DIGITS)
      {
        if (isdecimal)
        {
          exp--;
        }
        intval = (((intval << 2) + intval) << 1) + c; // intval*10 + c
      }
      else
      {
        if (!(isdecimal))
        {
          exp++;
        } // Drop overflow digits
      }
    }
    else if (c == (('.' - '0') & 0xff) && !(isdecimal))
    {
      isdecimal = true;
    }
    else
    {
      break;
    }
    c = *ptr++;
  }

  // Return if no digits have been read.
  if (!ndigit)
  {
    return (false);
  };

  // Convert integer into floating point.
  float fval;
  fval = (float)intval;

  // Apply decimal. Should perform no more than two floating point multiplications for the
  // expected range of E0 to E-4.
  if (fval != 0)
  {
    while (exp <= -2)
    {
      fval *= 0.01f;
      exp += 2;
    }
    if (exp < 0)
    {
      fval *= 0.1f;
    }
    else if (exp > 0)
    {
      do
      {
        fval *= 10.0f;
      } while (--exp > 0);
    }
  }

  // Assign floating point value with correct sign.
  if (isnegative)
  {
    *float_ptr = -fval;
  }
  else
  {
    *float_ptr = fval;
  }

  *char_counter = ptr - line - 1; // Set char_counter to next statement

  return (true);
}

// Simple hypotenuse computation function.
float hypot_f(float x, float y) { return (sqrtf(x * x + y * y)); }

float convert_delta_vector_to_unit_vector(float *vector)
{
  uint8_t idx;
  float magnitude = 0.0f;
  for (idx = 0; idx < N_AXIS; idx++)
  {
    if (vector[idx] != 0.0f)
    {
      magnitude += vector[idx] * vector[idx];
    }
  }
  magnitude = sqrtf(magnitude);
  float inv_magnitude = 1.0f / magnitude;
  for (idx = 0; idx < N_AXIS; idx++)
  {
    vector[idx] *= inv_magnitude;
  }
  return (magnitude);
}

float limit_value_by_axis_maximum(float *max_value, float *unit_vec)
{
  uint8_t idx;
  float limit_value = SOME_LARGE_VALUE;
  for (idx = 0; idx < N_AXIS; idx++)
  {
    if (unit_vec[idx] != 0)
    { // Avoid divide by zero.
      limit_value = min(limit_value, fabsf(max_value[idx] / unit_vec[idx]));
    }
  }
  return (limit_value);
}

/* Compute the MODBUS RTU CRC */
uint16_t ModRTU_CRC(uint8_t *buf, int len)
{
  uint16_t crc = 0xFFFF;

  for (int pos = 0; pos < len; pos++)
  {
    crc ^= (uint16_t)buf[pos]; // XOR byte into least sig. byte of crc

    for (int i = 8; i != 0; i--)
    { // Loop over each bit
      if ((crc & 0x0001) != 0)
      {            // If the LSB is set
        crc >>= 1; // Shift right and XOR 0xA001
        crc ^= 0xA001;
      }
      else         // Else LSB is not set
        crc >>= 1; // Just shift right
    }
  }
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
  return crc;
}

uint32_t hash(char *str)
{
  uint32_t hash = 5381;
  uint8_t c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}

void upperString(uint8_t *s)
{
  for (int i = 0; s[i] != '\0'; i++)
  {
    if (s[i] >= 'a' && s[i] <= 'z')
    {
      s[i] = s[i] - 32;
    }
  }
}

/**
 * @brief Separates the string into substrings. Remember to free memory for each string, and
 *        the array of pointers to strings
 *
 * @param string
 * @param seperators for example ",. _"
 * @param count return the count of strings via pass-by-pointer using the parameter count
 * @return char** An array of pointers to strings, dynamically allocated on the heap
 */
char **split(char *string, char *seperators, int *count)
{
  // get the length of the string
  int len = strlen(string);

  // use count to keep a count of the number of substrings
  *count = 0;

  // We make one pass of the string to first determine how many substrings
  // we'll need to create, so we can allocate space for a large enough array
  // of pointer to strings.  The variable i will keep track of our current
  // index in the string
  int i = 0;
  while (i < len)
  {
    // skip over the next group of separator characters
    while (i < len)
    {
      // keep incrementing i until the character at index i is NOT found in the
      // separators array, indicating we've reached the next substring to create
      if (strchr(seperators, string[i]) == NULL)
        break;
      i++;
    }

    // skip over the next group of substring (i.e. non-separator characters),
    // we'll use old_i to verify that we actually did detect non-separator
    // characters (perhaps we're at the end of the string)
    int old_i = i;
    while (i < len)
    {
      // increment i until the character at index i IS found in the separators
      // array, indicating we've reached the next group of separator
      // character(s)
      if (strchr(seperators, string[i]) != NULL)
        break;
      i++;
    }

    // if we did encounter non-seperator characters, increase the count of
    // substrings that will need to be created
    if (i > old_i)
      *count = *count + 1;
  }

  // allocate space for a dynamically allocated array of *count* number of
  // pointers to strings
  char **strings = malloc(sizeof(char *) * *count);

  // we'll make another pass of the string using more or less the same logic as
  // above, but this time we'll dynamically allocate space for each substring
  // and store the substring into this space
  i = 0;

  // buffer will temporarily store each substring, string_index will keep track
  // of the current index we are storing the next substring into using the
  // dynamically allocated array above
  char buffer[2 * len];
  int string_index = 0;
  while (i < len)
  {
    // skip through the next group of separators, exactly the same as above
    while (i < len)
    {
      if (strchr(seperators, string[i]) == NULL)
        break;
      i++;
    }

    // store the next substring into the buffer char array, use j to keep
    // track of the index in the buffer array to store the next char
    int j = 0;
    while (i < len)
    {
      if (strchr(seperators, string[i]) != NULL)
        break;

      buffer[j] = string[i];
      i++;
      j++;
    }

    // only copy the substring into the array of substrings if we actually
    // read in characters with the above loop... it's possible we won't if
    // the string ends with a group of separator characters!
    if (j > 0)
    {
      // add a null terminator on to the end of buffer to terminate the string
      buffer[j] = '\0';

      // calculate how much space to allocate... we need to be able to store
      // the length of buffer (including a null terminator) number of characters
      int to_allocate = sizeof(char) *
                        (strlen(buffer) + 1);

      // allocate enough space using malloc, store the pointer into the strings
      // array of pointers at hte current string_index
      strings[string_index] = malloc(to_allocate);

      // copy the buffer into this dynamically allocated space
      strcpy(strings[string_index], buffer);

      // advance string_index so we store the next string at the next index in
      // the strings array
      string_index++;
    }
  }

  // return our array of strings
  return strings;
}

STRING_NUM_TYPE_HandleTypedef string_number_type(char *string)
{
  if (string == NULL || *string == '\0')
    return IsNotNumber;

  bool have_dot = false, have_plus = false, have_minus = false;

  /*Check first character */
  char c = *string++;
  switch (c)
  {
  case '+':;
    have_plus = true;
    break;
  case '-':;
    have_minus = true;
    break;
  case '.':;
    have_dot = true;
  default:;
    if (c < '0' || c > '9')
      return false;
  }

  /* If no more characters, and the only character is +,-, or . */
  if (!strlen(string) && (have_plus + have_minus + have_dot))
    return IsNotNumber;

  /* Check the rest of the string */
  while ((c = *string++))
  {
    switch (c)
    {
    case '.':;
      if (have_dot)
        return IsNotNumber;
      else
        have_dot = true;
      break;
    default:;
      if (c < '0' || c > '9')
        return false;
    }
  }
  if (have_dot)
    return IsFloat;
  return IsInteger;
}

void reverse_float(float *data, uint8_t n)
{
    for (int i = 0; i < n / 2; i++)
    {
        float tmp = data[i];
        data[i] = data[n - 1 - i];
        data[n - 1 - i] = tmp;
    }
}

void reverse_int16(int16_t *data, uint8_t n)
{
    for (int i = 0; i < n / 2; i++)
    {
        int16_t tmp = data[i];
        data[i] = data[n - 1 - i];
        data[n - 1 - i] = tmp;
    }
}

void reverse_int32(int32_t *data, uint8_t n)
{
    for (int i = 0; i < n / 2; i++)
    {
        int32_t tmp = data[i];
        data[i] = data[n - 1 - i];
        data[n - 1 - i] = tmp;
    }
}

void reverse_int(int *data, uint8_t n)
{
    for (int i = 0; i < n / 2; i++)
    {
        int tmp = data[i];
        data[i] = data[n - 1 - i];
        data[n - 1 - i] = tmp;
    }
}