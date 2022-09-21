#pragma once

#include "gconsole.h"

namespace demoart
{
	static void displayASCIIArt(Console &acons)
	{

		//acons.putsln("01234567891123456789212345678931234567894123456789512345678961234567897123456789");
		//acons.putsln("          0         0         0         0         0         0         0         ");
		//acons.printf("The quick brown fox jumps over the lazy dog.");
		//acons.newLine();

		acons.putsln(",---,---,---,---,---,---,---,---,---,---,---,---,---,-------,");
		acons.putsln("|1/2| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 0 | + | ' | <-    |");
		acons.putsln("|---'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-----|");
		acons.putsln("| ->| | Q | W | E | R | T | Y | U | I | O | P | ] | ^ |     |");
		acons.putsln("|-----',--',--',--',--',--',--',--',--',--',--',--',--'|    |");
		acons.putsln("| Caps | A | S | D | F | G | H | J | K | L | \\ | [ | * |    |");
		acons.putsln("|----,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'---'----|");
		acons.putsln("|    | < | Z | X | C | V | B | N | M | , | . | - |          |");
		acons.putsln("|----'-,-',--'--,'---'---'---'---'---'---'-,-'---',--,------|");
		acons.putsln("| ctrl |  | alt |                          |altgr |  | ctrl |");
		acons.putsln("'------'  '-----'--------------------------'------'  '------'");

	}
}