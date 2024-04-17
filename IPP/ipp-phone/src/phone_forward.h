/** @file
 * Interfejs klasy przechowującej przekierowania numerów telefonicznych
 *
 * @author Marcin Peczarski <marpe@mimuw.edu.pl>
 * @author Michał Płachta <mp438675@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#ifndef __PHONE_FORWARD_H__
#define __PHONE_FORWARD_H__

#include <stdbool.h>
#include <stddef.h>

/**
 * To jest struktura przechowująca przekierowania numerów telefonów.
 */
struct PhoneForward;
typedef struct PhoneForward PhoneForward;

/**
 * To jest struktura przechowująca ciąg numerów telefonów.
 * W strukturze numery nie powtarzają się.
 */
struct PhoneNumbers;
typedef struct PhoneNumbers PhoneNumbers;

/** @brief Tworzy nową strukturę.
 * Tworzy nową strukturę niezawierającą żadnych przekierowań.
 * @return Wskaźnik na utworzoną strukturę lub NULL, gdy nie udało się
 *         alokować pamięci.
 */
PhoneForward * phfwdNew(void);

/** @brief Usuwa strukturę.
 * Usuwa strukturę wskazywaną przez @p pf. Nic nie robi, jeśli wskaźnik ten ma
 * wartość NULL.
 * @param[in] pf – wskaźnik na usuwaną strukturę.
 */
void phfwdDelete(PhoneForward *pf);

/** @brief Dodaje przekierowanie.
 * Dodaje przekierowanie wszystkich numerów mających prefiks @p num1, na numery,
 * w których ten prefiks zamieniono odpowiednio na prefiks @p num2. Każdy numer
 * jest swoim własnym prefiksem. Jeśli wcześniej zostało dodane przekierowanie
 * z takim samym parametrem @p num1, to jest ono zastępowane.
 * Relacja przekierowania numerów nie jest przechodnia.
 * @param[in,out] pf – wskaźnik na strukturę przechowującą przekierowania
 *                     numerów;
 * @param[in] num1   – wskaźnik na napis reprezentujący prefiks numerów
 *                     przekierowywanych;
 * @param[in] num2   – wskaźnik na napis reprezentujący prefiks numerów,
 *                     na które jest wykonywane przekierowanie.
 * @return Wartość @p true, jeśli przekierowanie zostało dodane.
 *         Wartość @p false, jeśli wystąpił błąd, np. podany napis nie
 *         reprezentuje numeru, oba podane numery są identyczne lub nie udało
 *         się alokować pamięci.
 */
bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2);

/** @brief Usuwa przekierowania.
 * Usuwa wszystkie przekierowania, w których parametr @p num jest prefiksem
 * parametru @p num1 użytego przy dodawaniu. Jeśli nie ma takich przekierowań
 * lub napis nie reprezentuje numeru, nic nie robi.
 * @param[in,out] pf – wskaźnik na strukturę przechowującą przekierowania
 *                     numerów;
 * @param[in] num    – wskaźnik na napis reprezentujący prefiks numerów.
 */
void phfwdRemove(PhoneForward *pf, char const *num);

/** @brief Wyznacza przekierowanie numeru.
 * Wyznacza przekierowanie podanego numeru. Szuka najdłuższego pasującego
 * prefiksu. Wynikiem jest ciąg zawierający co najwyżej jeden numer. Jeśli dany
 * numer nie został przekierowany, to wynikiem jest ciąg zawierający ten numer.
 * Jeśli podany napis nie reprezentuje numeru, wynikiem jest pusty ciąg.
 * Alokuje strukturę @p PhoneNumbers, która musi być zwolniona za pomocą
 * funkcji @ref phnumDelete.
 * @param[in] pf  – wskaźnik na strukturę przechowującą przekierowania numerów;
 * @param[in] num – wskaźnik na napis reprezentujący numer.
 * @return Wskaźnik na strukturę przechowującą ciąg numerów lub NULL, gdy nie
 *         udało się alokować pamięci.
 */
PhoneNumbers * phfwdGet(PhoneForward const *pf, char const *num);

/** @brief Wyznacza aktualne i ukryte przekierowania na dany numer.
 * Wyznacza następujący ciąg numerów: jeśli istnieje numer @p x, taki że wynik
 * wywołania @p phfwdGet z numerem @p x zawiera numer @p num lub po odpowiednich 
 * operacjach @p phfwdRemove taki numer @p x by istniał, to numer @p x
 * należy do wyniku wywołania @ref phfwdReverse z numerem @p num. Dodatkowo ciąg
 * wynikowy zawsze zawiera też numer @p num. Wynikowe numery są posortowane
 * leksykograficznie i nie mogą się powtarzać. Jeśli podany napis nie
 * reprezentuje numeru, wynikiem jest pusty ciąg. Alokuje strukturę
 * @p PhoneNumbers, która musi być zwolniona za pomocą funkcji @ref phnumDelete.
 * @param[in] pf  – wskaźnik na strukturę przechowującą przekierowania numerów;
 * @param[in] num – wskaźnik na napis reprezentujący numer.
 * @return Wskaźnik na strukturę przechowującą ciąg numerów lub NULL, gdy nie
 *         udało się alokować pamięci.
 */
PhoneNumbers * phfwdReverse(PhoneForward const *pf, char const *num);

/** @brief Usuwa strukturę.
 * Usuwa strukturę wskazywaną przez @p pnum. Nic nie robi, jeśli wskaźnik ten ma
 * wartość NULL.
 * @param[in] pnum – wskaźnik na usuwaną strukturę.
 */
void phnumDelete(PhoneNumbers *pnum);

/** @brief Udostępnia numer.
 * Udostępnia wskaźnik na napis reprezentujący numer. Napisy są indeksowane
 * kolejno od zera.
 * @param[in] pnum – wskaźnik na strukturę przechowującą ciąg numerów telefonów;
 * @param[in] idx  – indeks numeru telefonu.
 * @return Wskaźnik na napis reprezentujący numer telefonu. Wartość NULL, jeśli
 *         wskaźnik @p pnum ma wartość NULL lub indeks ma za dużą wartość.
 */
char const * phnumGet(PhoneNumbers const *pnum, size_t idx);

/** @brief Tworzy nową strukturę.
 * Tworzy nową strukturę niezawierającą żadnych numerów.
 * @return Wskaźnik na utworzoną strukturę lub NULL, gdy nie udało się
 *         alokować pamięci.
 */
PhoneNumbers * phnumNew();

/** @brief Dodaje numer do struktury.
 * Dodaje @p newNum do struktury @p pnums.
 * @param[in] pnums – wskaźnik na strukturę przechowującą ciąg numerów telefonów;
 * @param[in] newNum  – wskaźnik na dodawany numer telefonu
 */
void phnumAdd(PhoneNumbers* pnums, char const* newNum);

/** @brief Sprawdza czy numer należy do struktury.
 * Sprawdza czy @p number należy do struktury @p pnum.
 * @param[in] pnum – wskaźnik na strukturę przechowującą ciąg numerów telefonów;
 * @param[in] number  – wskaźnik na numer telefonu
 */
int phnumContains(PhoneNumbers* pnum, char const* number);

/** @brief Usuwa ze struktury numer telefonu.
 * Usuwa numer @p number ze struktury @p pnum.
 * @param[in] pnum – wskaźnik na strukturę przechowującą ciąg numerów telefonów;
 * @param[in] number  – wskaźnik na usuwany numer telefonu
 */
void phnumRemove(PhoneNumbers* pnum, char const* number);

/** @brief Sortuje numery w strukturze leksykograficznie.
 * Sortuje numery w strukturze @p pnum.
 * @param[in] pnum – wskaźnik na strukturę przechowującą ciąg numerów telefonów;
 */
void phnumSort(PhoneNumbers* pnum);

/** @brief Zwraca ilość elementów.
 * Zwraca ilość elementów w strukturze @p pnum.
 * @param[in] pnum – wskaźnik na strukturę przechowującą ciąg numerów telefonów;
 */
int phnumSize(PhoneNumbers* pnum);

/** @brief Wyznacza numery przekierowujące na @p num.
 * Zwraca strukturę @p PhoneNumbers, w której zawarte są te numery, które przekierowują na @p num.
 * Uporządkowane są w porządku leksykograficnzym.
 * @param[in] num – wskaźnik na numer telefonu, na który mają przekierowywać numery w strukturze wynikowej;
 * @return Wskaźnik na utworzoną strukturę @p PhoneNumbers zawierającą odpowiednie numery.
 */
PhoneNumbers * phfwdGetReverse(PhoneForward const *pf, char const *num);

#endif /* __PHONE_FORWARD_H__ */
