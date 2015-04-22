
#include <iostream>

inline int GetUtf8(const char* in, int& offset, int&utf8Offset) {
  int c = (unsigned char)in[offset++];
  ++utf8Offset;
  if (c < 0x80) { // 1-byte sequence
    return c;
  }
  else if ((c & 0xe0) == 0xc0) { // 2-byte sequence
    int c2 = (unsigned char)in[offset++];
    if(c2 < 0) return 0;
    return (((c & 0x3f) << 6) + (c2 & 0x3f));
  }
  else if ((c & 0xf0) == 0xe0) { // 3-byte sequence
    int c2 = (unsigned char)in[offset++];
    if(c2 < 0) return 0;
    int c3 = (unsigned char)in[offset++];
    if(c3 < 0) return 0;
    return  (((c & 0x1f) << 12) + ((c2 & 0x3f) << 6) + (c3 & 0x3f));
  }
  else if ((c & 0xf8) == 0xf0) { // 4-byte sequence
    int c2 = (unsigned char)in[offset++];
    if(c2 < 0) return 0;
    int c3 = (unsigned char)in[offset++];
    if(c3 < 0) return 0;
    int c4 = (unsigned char)in[offset++];
    if(c4 < 0) return 0;
    return (((c & 0x0f) << 18) + ((c2 & 0x3f) << 12) + ((c3 & 0x3f) << 6) + (c4 & 0x3f));
  }
  else {
    return 0; // Illegal UTF8
  }
}


//   GetUtf8 IS REPAIRED; BUT THE OTHER METHODS ARE BUGGY FOR 3- and 4-BYTE SEQUENCES!!!

// inline int ReadUtf8(std::istream& in) {
//       int c = in.get();
//       if(c == -1) {
//         return -1;
//       }
//       if (c < 0x80) {
//         return c;
//       }
//       else if ((c & 0xe0) == 0xc0) {
//         int c2 = in.get();
//         if(c2 < 0) return -1;
//         return (((c & 0x3f) << 6) + (c2 & 0x3f));
//       }
//       else if ((c & 0xe0) == 0xc0) {
//         int c2 = in.get();
//         if(c2 < 0) return -1;
//         int c3 = in.get();
//         if(c3 < 0) return -1;
//         return  (((c & 0x3f) << 12) + ((c2 & 0x3f) << 6) + (c3 & 0x3f));
//       }
//       else if ((c & 0xe0) == 0xc0) {
//         int c2 = in.get();
//         if(c2 < 0) return -1;
//         int c3 = in.get();
//         if(c3 < 0) return -1;
//         int c4 = in.get();
//         if(c4 < 0) return -1;
//         return (((c & 0x3f) << 18) + ((c2 & 0x3f) << 12) + ((c3 & 0x3f) << 6) + (c4 & 0x3f));
//       }
//       else {
//         return -1; // Illegal Utf8
//       }
// }

// inline void PrintUtf8(char*buf, int &pos, int codePoint) {
//   if (codePoint < 0x80) {
//      // one byte
//      buf[pos++] = (char)codePoint;
//   }
//   else if (codePoint < 0x800)
//   {
//      // two bytes
//      buf[pos++] = (char)(0xc0 | (codePoint >> 6));
//      buf[pos++] = (char)(0x80 | (codePoint & 0x3f));
//   }
//   else if (codePoint < 0x10000)
//   {
//      // three bytes
//      buf[pos++] = (char)(0xe0 | (codePoint >> 12));
//      buf[pos++] = (char)(0x80 | ((codePoint >> 6) & 0x3f));
//      buf[pos++] = (char)(0x80 | (codePoint & 0x3f));
//   }
//   else if (codePoint <= 0x10ffff)
//   {
//      // four bytes
//      buf[pos++] = (char)(0xf0 | (codePoint >> 18));
//      buf[pos++] = (char)(0x80 | ((codePoint >> 12) & 0x3f));
//      buf[pos++] = (char)(0x80 | ((codePoint >> 6) & 0x3f));
//      buf[pos++] = (char)(0x80 | (codePoint & 0x3f));
//   }
// }

// inline void PrintUtf8(std::ostream &out, int codePoint) {
//   if (codePoint < 0x80) {
//      // one byte
//      out << (char)codePoint;
//   }
//   else if (codePoint < 0x800)
//   {
//      // two bytes
//      out << (char)(0xc0 | (codePoint >> 6));
//      out << (char)(0x80 | (codePoint & 0x3f));
//   }
//   else if (codePoint < 0x10000)
//   {
//      // three bytes
//      out << (char)(0xe0 | (codePoint >> 12));
//      out << (char)(0x80 | ((codePoint >> 6) & 0x3f));
//      out << (char)(0x80 | (codePoint & 0x3f));
//   }
//   else if (codePoint <= 0x10ffff)
//   {
//      // four bytes
//      out << (char)(0xf0 | (codePoint >> 18));
//      out << (char)(0x80 | ((codePoint >> 12) & 0x3f));
//      out << (char)(0x80 | ((codePoint >> 6) & 0x3f));
//      out << (char)(0x80 | (codePoint & 0x3f));
//   }
// }


