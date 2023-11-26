#include <iostream>
#include <ctime>
#include <string>
#include <bitset>
#include <cstdlib>

class MHKnapsack {
public:
   MHKnapsack() {
      generateKeys();
   }

   void generateKeys() {

      int* W = new int[keySize];

      int sum = 0;
      for (int i = 0; i < keySize; ++i) {
         W[i] = sum + std::rand() % MAX_STEP + 1;  // Генерация следующего элемента
         sum += W[i];
      }

      // Генерация больших простых чисел q и r
      q = generatePrimeNumber(sum + 1, sum * 2);

      bool isRPrime = false;
      while (!isRPrime) {
         r = generatePrimeNumber(1, q - 1);
         isRPrime = (gcd(r, q) == 1);
      }

      // Вычисление мультипликативного обратного r по модулю q
      m = calculateMultiplicativeInverse(r, q);

      // Генерация секретного ключа
      privateKey = new int[keySize];

      for (int i = 0; i < keySize; ++i) {
         privateKey[i] = W[i];
      }

      // Создание публичного ключа B
      publicKey = new int[keySize];
      for (int i = 0; i < keySize; ++i) {
         publicKey[i] = (r * W[i]) % q;
      }

      delete[] W;
   }

   int* encryptMessage(const std::string& message) {
      int messageSize = message.length();
      int* messageBits = stringToBits(message);

      int* encryptedSums = new int[messageSize];

      int index = 0;
      for (int i = 0; i < messageSize; i++) {
         int encryptedSum = 0;
         for (int j = 0; j < 8; j++) {
            encryptedSum += publicKey[j % keySize] * messageBits[index + j];
         }
         index += 8;
         encryptedSums[i] = encryptedSum;
      }

      delete[] messageBits;
      return encryptedSums;
   }

   std::string decryptMessage(int* encryptedMessage, int messageSize) {
      int* decryptedBits = new int[messageSize * 8];

      for (int i = 0; i < messageSize; i++) {
         int encryptCode = (encryptedMessage[i] * m) % q;
         int index = i * 8;
         for (int j = keySize - 1; j >= 0; --j) {
            if (encryptCode >= privateKey[j % keySize]) {
               decryptedBits[index + j] = 1;
               encryptCode -= privateKey[j % keySize];
            }
            else {
               decryptedBits[index + j] = 0;
            }
         }
      }

      std::string decryptedMessage = bitsToString(decryptedBits, messageSize * 8);

      delete[] decryptedBits;
      return decryptedMessage;
   }

   std::string printMessage(int* message, int len) {
      std::string str;
      for (int i = 0; i < len; i++) {
         str += std::to_string(message[i]);
         str += " ";
      }
      return str;
   }

private:
   const int keySize = 8; // Размер секретного ключа (8 бит)
   const int MAX_STEP = 2;
   int* privateKey;
   int* publicKey;
   int q;
   int r;
   int m; // Мультипликативное обратное r по модулю q

   bool isPrime(int n) {
      if (n <= 1) {
         return false;
      }

      for (int i = 2; i * i <= n; ++i) {
         if (n % i == 0) {
            return false;
         }
      }

      return true;
   }

   int generatePrimeNumber(int min, int max) {
      int number = min + (std::rand() % (max - min + 1));
      while (!isPrime(number)) {
         number = min + (std::rand() % (max - min + 1));
      }
      return number;
   }

   int gcd(int a, int b) {
      if (b == 0) {
         return a;
      }
      return gcd(b, a % b);
   }

   int calculateMultiplicativeInverse(int a, int m) {
      int m0 = m;
      int y = 0, x = 1;

      if (m == 1) {
         return 0;
      }

      while (a > 1) {
         int q = a / m;
         int t = m;

         m = a % m;
         a = t;
         t = y;

         y = x - q * y;
         x = t;
      }

      if (x < 0) {
         x += m0;
      }

      return x;
   }

   int* stringToBits(const std::string& str) {
      int* bits = new int[str.length() * 8];
      int index = 0;
      int strLength = str.length();
      for (int i = 0; i < strLength; ++i) {
         char c = str[i];
         std::bitset<8> byte(static_cast<unsigned char>(c));
         for (int j = 0; j < 8; j++) {
            bits[index + 7 - j] = byte[j];
         }
         index += 8;
      }
      return bits;
   }

   std::string bitsToString(int* bits, int length) {
      std::string str;
      for (int i = 0; i < length; i += 8) {
         std::bitset<8> byte;
         for (int j = 0; j < 8; ++j) {
            byte[j] = bits[i + 7 - j];
         }
         str += static_cast<char>(byte.to_ulong());
      }
      return str;
   }
};

int main() {

   // Инициализация генератора псевдослучайных чисел на основе времени
   std::srand(std::time(0));

   MHKnapsack cipher;

   std::string originalMessage(getenv("TEXT"));
   int* encryptedMessage = cipher.encryptMessage(originalMessage);

   std::cout << "Original Message: " << originalMessage << std::endl;

   std::cout << "Encrypted Message: " << cipher.printMessage(encryptedMessage, originalMessage.length()) << std::endl;

   // Дешифрование сообщения
   std::string decryptedMessage = cipher.decryptMessage(encryptedMessage, originalMessage.length());

   std::cout << "Decrypted Message: " << decryptedMessage << std::endl;

   return 0;
}