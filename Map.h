/*
   Tree v2: https://github.com/evilgeniuslabs/tree-v2
   Copyright (C) 2016 Jason Coon

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

const double radii[NUM_LEDS] = {
  255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 255.0, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 191.25, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 127.5, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 63.75, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
};

const uint8_t conicalRadii[NUM_LEDS] = {
  0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 1, 2, 2, 1, 1, 2, 2, 1, 1, 2, 2, 1, 1, 2, 2, 1, 1, 2, 2, 1, 1, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

const uint16_t angles[NUM_LEDS] = {
  0, 2, 4, 6, 8, 11, 13, 15, 17, 19, 22, 24, 26, 28, 31, 33, 35, 37, 39, 42, 44, 46, 48, 51, 53, 55, 57, 59, 62, 64, 66, 68, 70, 73, 75, 77, 79, 82, 84, 86, 88, 90, 93, 95, 97, 99, 102, 104, 106, 108, 110, 113, 115, 117, 119, 121, 124, 126, 128, 130, 133, 135, 137, 139, 141, 144, 146, 148, 150, 152, 155, 157, 159, 161, 164, 166, 168, 170, 172, 175, 177, 179, 181, 184, 186, 188, 190, 192, 195, 197, 199, 201, 203, 206, 208, 210, 212, 215, 217, 219, 221, 223, 226, 228, 230, 232, 235, 237, 239, 241, 243, 246, 248, 250, 252, 0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48, 51, 54, 57, 60, 63, 66, 69, 72, 75, 78, 81, 84, 87, 90, 93, 96, 99, 102, 105, 108, 111, 114, 117, 120, 123, 126, 129, 132, 135, 138, 141, 144, 147, 150, 153, 156, 159, 162, 165, 168, 171, 174, 177, 180, 183, 186, 189, 192, 195, 198, 201, 204, 207, 210, 213, 216, 219, 222, 225, 228, 231, 234, 237, 240, 243, 246, 249, 252, 0, 4, 9, 13, 18, 23, 27, 32, 37, 41, 46, 51, 55, 60, 64, 69, 74, 78, 83, 88, 92, 97, 102, 106, 111, 115, 120, 125, 129, 134, 139, 143, 148, 153, 157, 162, 166, 171, 176, 180, 185, 190, 194, 199, 203, 208, 213, 217, 222, 227, 231, 236, 241, 245, 250, 0, 7, 14, 21, 29, 36, 43, 51, 58, 65, 72, 80, 87, 94, 102, 109, 116, 123, 131, 138, 145, 153, 160, 167, 174, 182, 189, 196, 203, 211, 218, 225, 233, 240, 247, 0, 25, 51, 76, 102, 127, 153, 178, 204, 229
};

const uint8_t levelCount = 5;
const uint8_t ledsPerLevel[levelCount] = {
  115, 85, 55, 35, 10
};

const uint16_t levelStart[levelCount] = {
  0, 115, 200, 255, 290
};

const uint16_t levelEnd[levelCount] = {
  114, 199, 254, 289, 299
};

const uint16_t levels[NUM_LEDS] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4
};

const uint16_t zCoords[NUM_LEDS] = {
  0, 0, 1, 2, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 11, 12, 13, 14, 15, 16, 16, 17, 18, 19, 20, 21, 22, 22, 23, 24, 25, 26, 27, 28, 28, 29, 30, 31, 32, 33, 34, 34, 35, 36, 37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 45, 46, 47, 48, 49, 50, 51, 51, 52, 53, 54, 55, 56, 56, 57, 58, 59, 60, 61, 62, 62, 63, 64, 65, 66, 67, 68, 68, 69, 70, 71, 72, 73, 73, 74, 75, 76, 77, 78, 79, 79, 80, 81, 82, 83, 84, 84, 85, 86, 87, 88, 89, 90, 90, 91, 92, 93, 94, 95, 96, 96, 97, 98, 99, 100, 101, 101, 102, 103, 104, 105, 106, 107, 107, 108, 109, 110, 111, 112, 113, 113, 114, 115, 116, 117, 118, 118, 119, 120, 121, 122, 123, 124, 124, 125, 126, 127, 128, 129, 130, 130, 131, 132, 133, 134, 135, 135, 136, 137, 138, 139, 140, 141, 141, 142, 143, 144, 145, 146, 147, 147, 148, 149, 150, 151, 152, 152, 153, 154, 155, 156, 157, 158, 158, 159, 160, 161, 162, 163, 164, 164, 165, 166, 167, 168, 169, 169, 170, 171, 172, 173, 174, 175, 175, 176, 177, 178, 179, 180, 181, 181, 182, 183, 184, 185, 186, 186, 187, 188, 189, 190, 191, 192, 192, 193, 194, 195, 196, 197, 198, 198, 199, 200, 201, 202, 203, 203, 204, 205, 206, 207, 208, 209, 209, 210, 211, 212, 213, 214, 215, 215, 216, 217, 218, 219, 220, 220, 221, 222, 223, 224, 225, 226, 226, 227, 228, 229, 230, 231, 232, 232, 233, 234, 235, 236, 237, 237, 238, 239, 240, 241, 242, 243, 243, 244, 245, 246, 247, 248, 249, 249, 250, 251, 252, 253, 254
};

const uint16_t xCoords[NUM_LEDS] = {
  255, 255, 254, 253, 252, 250, 248, 246, 243, 240, 237, 233, 229, 224, 220, 215, 210, 204, 199, 193, 187, 180, 174, 167, 161, 154, 147, 140, 133, 127, 120, 113, 106, 99, 92, 86, 79, 73, 67, 61, 55, 49, 44, 39, 34, 29, 25, 21, 17, 14, 11, 8, 6, 4, 3, 1, 1, 0, 0, 0, 1, 2, 3, 5, 7, 10, 13, 16, 19, 23, 27, 32, 37, 42, 47, 52, 58, 64, 70, 76, 83, 89, 96, 103, 110, 116, 123, 130, 137, 144, 151, 158, 165, 171, 178, 184, 190, 196, 202, 207, 213, 218, 222, 227, 231, 235, 239, 242, 245, 247, 249, 251, 253, 254, 255, 223, 223, 222, 221, 219, 217, 214, 211, 207, 203, 198, 193, 188, 183, 177, 170, 164, 157, 151, 144, 137, 130, 123, 116, 109, 102, 95, 89, 82, 76, 71, 65, 60, 55, 51, 47, 43, 40, 37, 35, 34, 33, 32, 32, 32, 33, 35, 37, 39, 42, 45, 49, 54, 58, 63, 69, 74, 80, 87, 93, 100, 107, 113, 120, 127, 135, 142, 148, 155, 162, 168, 175, 181, 186, 192, 197, 201, 206, 210, 213, 216, 218, 220, 222, 223, 191, 191, 190, 188, 185, 181, 177, 172, 167, 161, 154, 147, 140, 133, 126, 119, 112, 105, 98, 92, 86, 81, 76, 72, 69, 67, 65, 64, 64, 65, 66, 68, 72, 75, 80, 85, 91, 97, 103, 110, 117, 125, 132, 139, 146, 153, 159, 165, 171, 176, 180, 184, 187, 189, 191, 159, 159, 157, 155, 152, 147, 143, 137, 132, 126, 121, 115, 110, 106, 102, 99, 97, 96, 96, 97, 99, 101, 105, 110, 114, 120, 125, 131, 137, 142, 147, 151, 154, 157, 159, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128
};

const uint16_t yCoords[NUM_LEDS] = {
  128, 134, 141, 148, 155, 162, 168, 175, 181, 187, 194, 199, 205, 210, 216, 220, 225, 229, 233, 237, 240, 244, 246, 249, 251, 252, 253, 254, 255, 255, 255, 254, 253, 252, 250, 248, 246, 243, 240, 236, 232, 228, 224, 219, 214, 209, 203, 198, 192, 186, 180, 173, 167, 160, 153, 146, 139, 133, 126, 119, 112, 105, 98, 91, 85, 78, 72, 66, 60, 54, 49, 43, 38, 33, 29, 25, 21, 17, 14, 11, 8, 6, 4, 2, 1, 0, 0, 0, 0, 1, 2, 4, 5, 8, 10, 13, 16, 20, 24, 28, 32, 37, 42, 48, 53, 59, 65, 71, 77, 84, 90, 97, 104, 111, 117, 128, 135, 142, 148, 155, 162, 168, 175, 181, 186, 192, 197, 201, 206, 210, 213, 216, 218, 220, 222, 223, 223, 223, 222, 221, 220, 218, 215, 212, 208, 204, 200, 195, 190, 184, 179, 173, 166, 160, 153, 146, 139, 132, 125, 118, 111, 104, 98, 91, 85, 78, 72, 67, 62, 57, 52, 48, 44, 41, 38, 36, 34, 33, 32, 32, 32, 33, 34, 36, 38, 41, 44, 48, 52, 57, 62, 67, 72, 78, 85, 91, 98, 104, 111, 118, 128, 135, 142, 149, 156, 162, 168, 173, 178, 182, 185, 188, 190, 191, 191, 191, 189, 187, 184, 180, 176, 171, 165, 159, 153, 146, 139, 132, 125, 117, 110, 103, 97, 91, 85, 80, 75, 72, 68, 66, 65, 64, 64, 65, 66, 69, 72, 76, 81, 86, 92, 98, 105, 112, 119, 128, 133, 139, 144, 148, 152, 156, 158, 159, 159, 159, 157, 154, 151, 146, 142, 136, 131, 125, 119, 114, 109, 105, 101, 98, 97, 96, 96, 97, 99, 102, 106, 111, 116, 121, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128
};

uint16_t getNearestToAngleAndLevel(uint8_t angle, uint8_t level) {
  uint8_t smallestDifference = 255;
  uint8_t nearestIndex = 0;

  for(uint16_t i = 0; i < NUM_LEDS; i++) {
    if(levels[i] != level)
      continue;

    uint8_t currentAngle = angles[i];

    uint8_t difference = abs(currentAngle - angle);

    if(difference > smallestDifference)
      continue;
    else if (difference == smallestDifference && random8() > 127)
      continue;

    smallestDifference = difference;
    nearestIndex = i;
  }

  return nearestIndex;
}
