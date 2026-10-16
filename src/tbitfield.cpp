// ННГУ, ВМК, Курс "Методы программирования-2", С++, ООП
//
// tbitfield.cpp - Copyright (c) Гергель В.П. 07.05.2001
//   Переработано для Microsoft Visual Studio 2008 Сысоевым А.В. (19.04.2015)
//
// Битовое поле

#include "tbitfield.h"

// Fake variables used as placeholders in tests
static const int FAKE_INT = -1;
static TBitField FAKE_BITFIELD(1);

TBitField::TBitField(int len)
{
    if (len < 0){
        throw std::length_error("Error, negative lenght");
    lenBit = len;}
    if (BitLen %(8*sizeof(TELEM)) == 0) {
        MemL = lenBit /(8*sizeof(TELEM));
    }
    else {
        MemL = lenBit/(8*sizeof(TELEM))+1;
    }
    pMem = new TELEM[MemL];
    for (int i = 0; i<MemL; i++) {
        pMem[i] = 0;
    }
}

TBitField::TBitField(const TBitField &bf) // конструктор копирования
{
    lenBit = bf.lenBit;
    MemL = bf.MemoryL;
    pMem = new TELEM[MemL];
    for (int i = 0; i<MemL; i++)
        pMem[i] = bf.pMem[i];
}

TBitField::~TBitField()
{
    delete[] pMem;
    pMem = nullptr;
    lenBit = 0;
    MemL = 0;
}

int TBitField::GetMemIndex(const int n) const // индекс Мем для бита n
{
    if ((0<= n) and (n <= lenBit-1)) {
        int index = n / (8*sizeof(TELEM));
        return index;
    }
    else{
        throw std::out_of_range("Error, index out of range");}
}

TELEM TBitField::GetMemMask(const int n) const // битовая маска для бита n
{
    if ((0<= n) and (n <= lenBit-1)) {
        int j = n % (8*sizeof(TELEM));
        return ((TELEM)1 << j);
    }
    else{
        throw std::out_of_range("Error, index out of range");}
}

// доступ к битам битового поля

int TBitField::GetLength(void) const // получить длину (к-во битов)
{
  return lenBit;
}

void TBitField::SetBit(const int n) // установить бит
{
    if ((0<= n) and (n <= lenBit-1)) {
        TELEM mask = GetMemMask(n);
        int val = GetMemIndex(n);
        pMem[val] = pMem[val] | mask;
    }
    else{
        throw std::out_of_range("Error, index out of range");}
}

void TBitField::ClrBit(const int n) // очистить бит
{
    if ((0<= n) and (n <= lenBit-1)) {
        int val = GetMemIndex(n);
        TELEM mask = ~GetMemMask(n);
        pMem[val] = pMem[val] & mask;
    }
    else{
        throw std::out_of_range("Error, index out of range");}
}

int TBitField::GetBit(const int n) const // получить значение бита
{
    if ((0<= n) and (n <= lenBit-1)) {
        TELEM mask = GetMemMask(n);
        int val = GetMemIndex(n);
        if ((pMem[val] & mask) == 0)
            return 0;
        return 1;
    }
    else{
        throw std::out_of_range("Error, index out of range");}
}

// битовые операции

TBitField& TBitField::operator=(const TBitField &bf) // присваивание
{
    if (this == &bf)
        return *this;
    if (MemL == bf.MemLen) {
        for (int i = 0; i<MemL; i++)
            pMem[i] = bf.pMem[i];
    }
    else {
        TELEM *newMem = new TELEM[bf.MemL];
        for (int i = 0; i < bf.MemL; i++)
            newMem[i] = bf.pMem[i];
        delete[] pMem;
        pMem = newMem;
        MemL = bf.MemL;
    }
    lenBit = bf.lenBit;
    return *this;
}

int TBitField::operator==(const TBitField &bf) const // сравнение
{
    if (lenBit != bf.lenBit || MemL != bf.MemL)
        return 0;
    for (int i = 0; i<MemL; i++) {
        if (pMem[i] != bf.pMem[i])
            return 0;
    }
    return 1;
}

int TBitField::operator!=(const TBitField &bf) const // сравнение
{
    return !(*this == bf);
}

TBitField TBitField::operator|(const TBitField &bf) // операция "или"
{
    TBitField result(max(lenBit, bf.lenBit));
    for (int i=0; i<min(MemL, bf.MemL); i++) {
        result.pMem[i] = pMem[i] | bf.pMem[i];
    }
    for (int i = min(MemL, bf.MemL); i<max(MemL, bf.MemL); i++) {
        if (MemLen > bf.MemL)
            result.pMem[i] = pMem[i];
        else
            result.pMem[i] = bf.pMem[i];
    }
    return result;
}

TBitField TBitField::operator&(const TBitField &bf) // операция "и"
{
    TBitField result(max(lenBit, bf.lenBit));
    for (int i = 0; i<min(MemL, bf.MemL); i++)
        result.pMem[i] = pMem[i] & bf.pMem[i];
    return result;
}

TBitField TBitField::operator~(void) // отрицание
{
    TBitField result(lenBit);
    for (int i = 0; i<MemL; i++) {
        result.pMem[i] = ~pMem[i];
    }
    int tail = lenBit % (8*sizeof(TELEM));
    if (tail == 0)
        return result;
    result.pMem[result.MemL-1] &= (((TELEM)1 << tail) - 1);
    return result;
}

// ввод/вывод

istream &operator>>(istream &istr, TBitField &bf) // ввод
{
    TBitField result(bf.lenBit);
    for (int i = 0; i < bf.BitLen; ++i)
    {
        char ch;
        if (!(istr >> ch)) return istr;
        if (ch != '0' && ch != '1')
        {
            istr.setstate(ios::failbit);
            return istr;
        }
        if (ch == '1') result.SetBit(i);
    }
    bf = result;
    return istr;
}

ostream &operator<<(ostream &ostr, const TBitField &bf) // вывод
{
    for (int i = 0; i < bf.lenBit; ++i) ostr << bf.GetBit(i);
    return ostr;
}
