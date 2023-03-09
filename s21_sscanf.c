#include "s21_string.h"

int s21_sscanf(const char *str1, const char *format1, ...) {
  char str[2048];
  s21_strcpy(str, str1);
  char format[2048];
  s21_strcpy(format, format1);
  long int result = 0;
  int empty_string = 0;
  int mistake = 0;
  int space_f = 0;
  char *temp_s = (char *)str, *temp_f = (char *)format;
  va_list ap;
  va_start(ap, format1);
  int is_star = 0;
  while (*temp_f != '\0' && mistake != -1) {
    if (skip_space(&temp_f, &temp_f)) space_f = 1;
    if (*temp_f == '%') {
      temp_f += 1;
      struct characteristic C = {0};
      while (digit(*temp_f) || (*temp_f == '*') || is_hl(*temp_f)) {
        stars(&temp_f, &is_star);
        if (digit(*temp_f)) C.width = s21_atod(&temp_f, C, 100, &mistake);
        if (is_hl(*temp_f)) C.len = define_long(&temp_f, C);
      }
      if ((*temp_f != 'c') || (space_f)) skip_space(&temp_s, &temp_f);
      if (*temp_s || *temp_f == 'n') {
        parser_scanf(&temp_f, &temp_s, &result, ap, &mistake, C, str, is_star);
      } else if (!result) {
        empty_string = -1;
      }
      if (*temp_f) temp_f += 1;
      is_star = 0;
    } else if (*temp_s == *temp_f ||
               ((skip_space(&temp_s, &temp_f)) && *temp_s == *temp_f)) {
      temp_s++;
      if (*temp_f != '\0') temp_f++;
    } else {
      if (*temp_f != '\0') temp_f += 1;
      mistake = -1;
    }
    space_f = 0;
  }
  if (empty_string == -1) {
    result = -1;
  }
  va_end(ap);
  return result;
}

void parser_scanf(char **temp_f, char **temp_s, long int *result, va_list ap,
                  int *mistake, struct characteristic C, const char *str,
                  const int is_star) {
  long double final_value = 0;
  long long final_value_int = 0, n_counter = 0;
  unsigned long long final_value_uint = 0;
  char s_result[1024] = {0};
  int ox_read = 0;
  int sign = 1;
  switch (**temp_f) {
    case '%':
      count_(temp_s, mistake);
      break;
    case 'c':
      count_c(temp_s, s_result, C, ap, is_star, result);
      break;
    case 'd':
    case 'i':
      count_di(temp_s, temp_f, final_value_int, C, ap, mistake, is_star,
               result);
      break;
    case 'u':
    case 'o':
      count_uo(temp_s, temp_f, final_value_uint, C, ap, mistake, is_star,
               result);
      break;
    case 'X':
    case 'x':
    case 'p':
      count_xp(temp_s, final_value_uint, &C, ap, mistake, temp_f, is_star,
               result, &sign, &ox_read);
      break;
    case 'e':
    case 'E':
    case 'g':
    case 'G':
    case 'f':
      count_f(temp_s, final_value, C, ap, mistake, is_star, result);
      break;
    case 'n':
      count_n(temp_s, n_counter, C, ap, mistake, temp_f, is_star, str, result);
      break;
    case 's':
      count_s(temp_s, C, ap, s_result, mistake, temp_f, is_star, result);
      break;
    default:
      *mistake = -1;
      break;
  }
}

void count_(char **temp_s, int *mistake) {
  if (**temp_s == '%') {
    (*temp_s)++;
  } else
    *mistake = -1;
}

void count_c(char **temp_s, char *s_result, struct characteristic C, va_list ap,
             int is_star, long int *result) {
  if (!is_star) {
    if (C.width == 0) {
      *va_arg(ap, char *) = **temp_s;
    } else if (C.width > 0) {
      int i = 0;
      for (; C.width > 0; C.width--, i++) {
        s_result[i] = (*temp_s)[i];
      }
      s21_strcpy_govno(va_arg(ap, char *), s_result);
    }
    (*result)++;
  }
  (*temp_s)++;
}

void count_di(char **temp_s, char **temp_f, long long final_value_int,
              struct characteristic C, va_list ap, int *mistake, int is_star,
              long int *result) {
  if ((**temp_f) == 'd') {
    final_value_int = s21_atod(temp_s, C, **temp_f, mistake);
    if (!is_star && (*mistake) == 0)
      final_format_int(temp_f, &final_value_int, ap, mistake, result, C);
  } else if ((**temp_f) == 'i') {
    final_value_int = transfer_i(temp_s, C, mistake);
    if (!is_star)
      final_format_int(temp_f, &final_value_int, ap, mistake, result, C);
  }
}

void count_uo(char **temp_s, char **temp_f, unsigned long long final_value_uint,
              struct characteristic C, va_list ap, int *mistake, int is_star,
              long int *result) {
  if ((**temp_f) == 'u') {
    final_value_uint = transfer(temp_s, 10, C, mistake, 0);
    if (!is_star)
      final_format_uint(temp_f, &final_value_uint, ap, mistake, result, C);
  } else if ((**temp_f) == 'o') {
    final_value_uint = transfer(temp_s, 8, C, mistake, 0);
    if (!is_star)
      final_format_oxpint(temp_f, &final_value_uint, ap, mistake, result, C);
  }
}

void count_xp(char **temp_s, unsigned long long final_value_uint,
              struct characteristic *C, va_list ap, int *mistake, char **temp_f,
              int is_star, long *result, int *sign, int *ox_read) {
  if (*(*temp_s) == '-') {
    (*temp_s) += 1;
    (*sign) *= -1;
  }
  if (*(*temp_s) == '0' && is_x(*(*temp_s + 1))) {
    (*temp_s) += 2;
    (*ox_read) += 1;
    if (C->width != 0) {
      C->width -= 2;
      if (C->width == 0) {
        C->width -= 1;
      }
    }
  }
  if (*sign < 0) {
    (*temp_s) -= 1;
    *(*temp_s) = '-';
  }
  final_value_uint = transfer(temp_s, 16, *C, mistake, *ox_read);
  if (!is_star)
    final_format_oxpint(temp_f, &final_value_uint, ap, mistake, result, *C);
}

void count_f(char **temp_s, long double final_value, struct characteristic C,
             va_list ap, int *mistake, int is_star, long int *result) {
  final_value = s21_atof(temp_s, C, mistake);
  if (!is_star && (*mistake) == 0)
    final_format_float(&final_value, ap, mistake, result, C);
}

void count_n(char **temp_s, long long n_counter, struct characteristic C,
             va_list ap, int *mistake, char **temp_f, int is_star,
             const char *str, long int *result) {
  n_counter = ((*temp_s) - str);
  if (!n_counter) *mistake = 2323;
  if (!is_star) final_format_int(temp_f, &n_counter, ap, mistake, result, C);
}

void count_s(char **temp_s, struct characteristic C, va_list ap, char *s_result,
             int *mistake, char **temp_f, int is_star, long int *result) {
  s21_atos(temp_s, C, s_result, mistake, temp_f);
  if (!is_star) {
    s21_strcpy(va_arg(ap, char *), s_result);
  }
  if (!*mistake && !is_star) (*result)++;
  (*temp_s) += s21_strlen(s_result);
}

long long int s21_atod(char **str, struct characteristic C, char c,
                       int *mistake) {
  long long int result = 0;
  int sign = 1, base = 10, i = 0, full = 0;
  if (C.width == 0) C.width = INT_MAX;
  if (find_sign(str, C, &sign)) C.width--;
  for (; i < C.width && digit(**str); (*str)++, i++) {
    if (!full && ((result * base + (**str - 48) >= result) &&
                  (result * base + (**str - 48) <= LONG_MAX))) {
      result = result * base + (**str - 48);
    } else if (!full && c == 100) {
      sign == 1 ? (result = LONG_MAX) : (result = LONG_MIN);
      full = 1;
    }
  }
  if (i == 0) *mistake = -1;
  return full ? result : result * sign;
}

long double s21_atof(char **str, struct characteristic C, int *mistake) {
  int sign = 1, i = 0, full = 0, point = 0, check = 0;
  long double base = 10, result = 0;
  if (C.width == 0) C.width = INT_MAX;
  int marker_sign = find_sign(str, C, &sign);
  if (marker_sign) C.width--;
  check = check_inf_nan(str, &result, &C);
  if (is_e(**str) == 0 && check != 1) {
    for (; (i < C.width) && (digit(**str) || **str == 46 || is_e(**str));
         (*str)++, i++) {
      if (!full && **str == 46) {
        point = 1;
      } else if (!full && is_e(**str)) {
        C.width = (C.width - i - 1);
        if (C.width == 0 ||
            (C.width == 1 && (*(*str + 1) == '+' || *(*str + 1) == '-')))
          break;
        e_calulation(str, &result, C);
      } else if (!full && !point &&
                 ((result * base + (**str - 48) >= result) &&
                  (result * base + (**str - 48) <= LONG_MAX))) {
        result = result * base + (**str - 48);
      } else if (!full && point && (result + ((**str - 48) / base) >= result) &&
                 (result + ((**str - 48) / base) <= LONG_MAX)) {
        result = result + ((**str - 48) / base);
        base *= 10;
      } else if (!full) {
        sign == 1 ? (result = LONG_MAX) : (result = LONG_MIN);
        full = 1;
      }
    }
  } else if (check == 0) {
    (*str) += 1;
  }
  if (i == 0 && check == 0) *mistake = -1;
  return full ? result : result * sign;
}

void e_calulation(char **str, long double *result, struct characteristic C) {
  long double e_base = 0;
  (*str)++;
  int mistake = 0, full = 0, base = 10;
  if (**str == 43) {
    e_base = 10;
    (*str)++;
    mistake = !(digit(**str));
  } else if (**str == 45) {
    e_base = 0.1;
    (*str)++;
    mistake = !(digit(**str));
  } else if (digit(**str)) {
    e_base = 10;
  } else {
    mistake = 1;
  }
  if (!mistake) {
    long double exponenta = 0;
    for (int i = 0; i < C.width && digit(**str); (*str)++, i++) {
      if (!full && exponenta * base + (**str - 48) <= LONG_MAX &&
          exponenta * base + (**str - 48) >= 0) {
        exponenta = exponenta * base + (**str - 48);
      } else if (!full) {
        exponenta = LONG_MAX;
        full = 1;
      }
    }
    (*str)--;
    *result = (*result) * pow(e_base, exponenta);
  }
}

char *s21_atos(char **temp_s, struct characteristic C, char *s_result,
               int *mistake, char **temp_f) {
  skip_space(temp_s, temp_f);
  if (C.width == 0) C.width = INT_MAX;
  int i = 0;
  for (; i < C.width && *(*(temp_s) + i) != '\0' && find_space(temp_s, i);
       i++) {
    *(s_result + i) = *(*(temp_s) + i);
  }
  *(s_result + i) = '\0';
  if (i == 0) *mistake = -1;
  return s_result;
}

void final_format_int(char **temp_f, long long int *final_value, va_list ap,
                      int *mistake, long int *result, struct characteristic C) {
  if (C.len == 0) *va_arg(ap, int *) = (int)*final_value;
  if (C.len == 2) *va_arg(ap, long int *) = (long int)*final_value;
  if (C.len == 3) *va_arg(ap, short int *) = (short int)*final_value;
  if (C.len == 4) *va_arg(ap, char *) = (char)*final_value;
  if (C.len == 5) *va_arg(ap, long long int *) = (long long int)*final_value;
  if (!*mistake && **temp_f != 'n') (*result)++;
}

void final_format_oxpint(char **temp_f, unsigned long long int *final_value,
                         va_list ap, int *mistake, long int *result,
                         struct characteristic C) {
  if (**temp_f == 'p') {
    *va_arg(ap, unsigned long int *) = (unsigned long int)*final_value;
  } else if (C.len == 0) {
    *va_arg(ap, unsigned int *) = (unsigned int)*final_value;
  } else if (C.len == 3) {
    *va_arg(ap, unsigned short int *) = (unsigned short int)*final_value;
  } else if (C.len == 2) {
    *va_arg(ap, unsigned long int *) = (unsigned long int)*final_value;
  } else if (C.len == 4) {
    *va_arg(ap, char *) = (char)*final_value;
  } else if (C.len == 5) {
    *va_arg(ap, long long int *) = (long long int)*final_value;
  }
  if (!*mistake && **temp_f != 'n') (*result)++;
}

void final_format_uint(char **temp_f, unsigned long long int *final_value,
                       va_list ap, int *mistake, long int *result,
                       struct characteristic C) {
  if (C.len == 0) *va_arg(ap, unsigned int *) = (unsigned int)*final_value;
  if (C.len == 3)
    *va_arg(ap, unsigned short int *) = (unsigned short int)*final_value;
  if (C.len == 2)
    *va_arg(ap, unsigned long int *) = (unsigned long int)*final_value;
  if (C.len == 4) *va_arg(ap, unsigned char *) = (unsigned char)*final_value;
  if (C.len == 5)
    *va_arg(ap, long long unsigned int *) =
        (long long unsigned int)*final_value;
  if (!*mistake && **temp_f != 'n') (*result)++;
}

void final_format_float(long double *final_value, va_list ap, int *mistake,
                        long int *result, struct characteristic C) {
  if (C.len == 0) *va_arg(ap, float *) = (float)*final_value;
  if (C.len == 1) *va_arg(ap, long double *) = (long double)*final_value;
  if (C.len == 2) *va_arg(ap, double *) = (double)*final_value;
  if (!*mistake) (*result)++;
}

void stars(char **temp_f, int *is_star) {
  while (**temp_f == '*') {
    (*temp_f)++;
    (*is_star)++;  // В sscanf знак *, помещенный после % и перед спецификатором
                   // формата, считывает данные указанного типа, но подавляет их
                   // присваивание.
  }
}

int letter(int c) {
  return ((c >= 65 && c <= 70) || (c >= 97 && c <= 102)) ? 1 : 0;
}
int digit(int d) { return (d >= 48 && d <= 57) ? 1 : 0; }
int digit_eight(int d) { return (d >= 48 && d <= 55) ? 1 : 0; }
int is_e(int d) { return ((d == 69) || (d == 101)) ? 1 : 0; }
int is_x(int d) { return ((d == 88) || (d == 120)) ? 1 : 0; }
int is_hl(int d) {
  return ((d == 76) || (d == 108) || (d == 104)) ? 1 : 0;
}  // L l h

int find_sign(char **str, struct characteristic C, int *sign) {
  int res = 0;
  if (**str == '-') {
    *sign = -1;
    (*str)++;
    if (C.width != 0) res++;
  } else if (**str == '+') {
    (*str)++;
    if (C.width != 0) res++;
  }
  return res;
}

int skip_space(char **temp_s, char **temp_f) {
  int res = 0;
  if (*(*temp_f) != '\0') {
    while (**temp_s == ' ' || **temp_s == '\n' || **temp_s == '\t' ||
           **temp_s == '\r' || **temp_s == '\f' || **temp_s == '\v') {
      if (*(*temp_f + 1) == 'c') {
        break;
      }
      (*temp_s)++;
      res = 1;
    }
  }
  return res;
}

int find_space(char **temp_s, int i) {
  int res = 1;
  *(temp_s) += i;
  if (**temp_s == ' ' || **temp_s == '\n' || **temp_s == '\t' ||
      **temp_s == '\r' || **temp_s == '\f' || **temp_s == '\v') {
    res = 0;
  }
  *(temp_s) -= i;
  return res;
}

int define_long(char **temp_f, struct characteristic C) {
  switch (**temp_f) {
    case 'L':
      C.len = 1;
      break;
    case 'l':
      C.len = 2;
      if (*(*temp_f + 1) == 'l') {
        C.len = 5;
        (*temp_f)++;
      }
      break;
    case 'h':
      C.len = 3;
      if (*(*temp_f + 1) == 'h') {
        C.len = 4;
        (*temp_f)++;
      }
      break;
  }
  (*temp_f)++;
  return C.len;
}

int check_inf_nan(char **str, long double *res, struct characteristic *C) {
  char temp[20] = {0};
  int check = 1;
  s21_strncpy(temp, *str, 10);
  char *low_temp = s21_to_lower(temp);
  if (C->width > 2 && !s21_strncmp(low_temp, "nan", 3)) {
    *res = 0.0 / 0.0;
    *str += 3;
    C->width -= 3;
  } else if (C->width > 7 && !s21_strncmp(low_temp, "infinity", 8)) {
    *res = 1.0 / 0.0;
    *str += 8;
    C->width -= 8;
  } else if (C->width > 2 && !s21_strncmp(low_temp, "inf", 3)) {
    *res = 1.0 / 0.0;
    *str += 3;
    C->width -= 3;
  } else {
    check = 0;
  }
  free(low_temp);
  return check;
}

char *s21_strcpy_govno(char *dest, const char *src) {
  char *ret = dest;
  while (*src != '\0') {
    *dest = *src;
    ++dest;
    ++src;
  }
  return ret;
}

unsigned long long transfer(char **number, int system, struct characteristic C,
                            int *mistake, int result) {
  long long unsigned int res = 0;
  int k = 0, sign = 1;
  if (C.width == 0) C.width = INT_MAX;
  if (find_sign(number, C, &sign)) C.width -= 1;
  int i = 0;
  for (;
       i < C.width && ((digit(**number) && (system == 10)) ||
                       (digit_eight(**number) && system == 8) ||
                       ((letter(**number) || digit(**number)) && system == 16));
       i++, (*number)++) {
    k = **number;
    if (system == 16) {
      if (k >= 65 && k <= 70) {
        k -= 7;
      } else if (k >= 97 && k <= 102) {
        k -= 39;
      }
    }
    k -= '0';
    if ((ULLONG_MAX / 10) != res) {
      res = res * system + k;
    } else {
      res = ULLONG_MAX;
    }
  }
  if (i == 0 && result == 0) *mistake = -1;
  return sign * res;
}

long long int transfer_i(char **temp_s, struct characteristic C, int *mistake) {
  long long int result = 0;
  int base = 10, k = 0, sign = 1, full = 0;
  if (C.width == 0) C.width = INT_MAX;
  if (find_sign(temp_s, C, &sign)) C.width -= 1;
  if (**temp_s == '0') base = 8;
  if (**temp_s == '0' && is_x(*(*temp_s + 1))) base = 16;
  if (base == 16 && (C.width > 2)) {
    C.width -= 2;
    (*temp_s) += 2;
  }
  int i = 0;
  for (; i < C.width && !full &&
         ((digit(**temp_s) && base == 10) ||
          (digit_eight(**temp_s) && base == 8) ||
          ((digit(**temp_s) || letter(**temp_s)) && base == 16));
       i++, (*temp_s)++) {
    k = **temp_s;
    if (k >= 65 && k <= 70 && base == 16) {
      k -= 7;
    } else if (k >= 97 && k <= 102 && base == 16) {
      k -= 39;
    }
    k -= '0';
    if (!full &&
        ((result * base + k >= result) && (result * base + k <= LONG_MAX))) {
      result = result * base + k;
    } else if (!full) {
      sign == 1 ? (result = LONG_MAX) : (result = LONG_MIN);
      full = 1;
    }
  }
  if (i == 0) *mistake = -1;
  return full ? result : result * sign;
}
