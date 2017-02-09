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
    comma = Literal(",").setParseAction(replaceWith("."))
    semicolon = Suppress(";")

    element = Word( caps, max=1 ) | Word( lowers, max=1 )
    integer = Word( digits )
    floa = Combine( integer + Optional( comma + integer) + Optional ( dot + integer ))
    elementRef = element + Optional( floa )
    #go = Group(goto + parenthesesL + OneOrMore( elementRef + Optional(semicolon)) + parenthesesR)
    #command = go + ZeroOrMore(go)
    command = goto + parenthesesL + OneOrMore( elementRef + Optional(semicolon)) + parenthesesR
    formulaData = command.parseString(s)

    return formulaData


print parseString("gOto(X23,4; y234.234; s345)")
