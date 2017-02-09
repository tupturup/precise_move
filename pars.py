from pyparsing import Word, Optional, OneOrMore, Group, ParseException, Combine, CaselessLiteral, Suppress, ZeroOrMore, Literal, replaceWith
import pyparsing

def parseString(s):
    goto = CaselessLiteral("goto")
    caps = "GOTABCSXYZ"
    lowers = caps.lower()
    digits = "0123456789"
    parenthesesL = Suppress("(")
    parenthesesR = Suppress(")")
    dot = "."
    minus = "-"
    comma = Literal(",").setParseAction(replaceWith("."))
    semicolon = Suppress(";")

    element = Word( caps, max=1 ) | Word( lowers, max=1 )
    number = Word( digits )
    integer = Optional(minus)  + number
    floa = Combine( integer + Optional( comma + number) + Optional ( dot + number ))
    elementRef = element + Optional( floa )
    go = Group(goto + parenthesesL + OneOrMore( elementRef + Optional(semicolon)) + parenthesesR)
    command = go + ZeroOrMore(go)
    #command = goto + parenthesesL + OneOrMore( elementRef + Optional(semicolon)) + parenthesesR
    while True:
        try:
            formulaData = command.parseString(s)
            break
        except ValueError:
            return "error"

    return formulaData


print parseString("gOto(X-23,4; y234.234; s345) goto(X234; Y563)")
