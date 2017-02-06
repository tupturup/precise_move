from pyparsing import Word, Optional, OneOrMore, Group, ParseException, Combine, CaselessLiteral, Suppress
import pyparsing

def parseString(s):
    goto = CaselessLiteral("goto")
    caps = "GOTABCSXYZ"
    lowers = caps.lower()
    digits = "0123456789"
    parenthesesL = Suppress("(")
    parenthesesR = Suppress(")")
    dot = "."
    comma = ","
    semicolon = Suppress(";")

    element = Word( caps, max=1 ) | Word( lowers, max=1 )
    integer = Word( digits )
    floa = Combine( integer + Optional( dot | comma + integer))
    elementRef = element + Optional( floa )
    command = goto + parenthesesL + OneOrMore( elementRef + Optional(semicolon)) + parenthesesR

    formulaData = command.parseString(s)
    return formulaData
print parseString("goto(a234; x234)")
