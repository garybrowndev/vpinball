#pragma once

class Kerning final {
public:
   Kerning();
   ~Kerning();

   int GetHash();

   char GetFirstCharacter() const { return m_firstCharacter; }
   void SetFirstCharacter(char firstCharacter) { m_firstCharacter = firstCharacter; }
   char GetSecondCharacter() const { return m_secondCharacter; }
   void SetSecondCharacter(char secondCharacter) { m_secondCharacter = secondCharacter; }
   int GetAmount() const { return m_amount; }
   void SetAmount(int amount) { m_amount = amount; }

private:
   char m_firstCharacter;
   char m_secondCharacter;
   int m_amount;
};
