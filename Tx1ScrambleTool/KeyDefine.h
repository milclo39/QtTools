#ifndef KEYDEFINE_H
#define KEYDEFINE_H

#include <QString>
#include <QByteArray>

#define TEST_SECRET_STRING "MYSECRET"

static const char PUBKEY[] =
	"-----BEGIN PUBLIC KEY-----\n"
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAqw54iD5fr9DZPmykD4lJ\n"
"+sRT7c9mRIu0p66sshX9l6aD4G/xuyQ3cf2x44VgL4CjiIrBqUAKtZ12/AAirbjj\n"
"ZMF6zMcf/b7kF4QMkKR96DJ4hb/lZy+5YkJENA6SoEhlkOUkPeApdDu0e/cUptvJ\n"
"JXuzbN6WGP4/2SVUkrGaJhjdSa5Z+KB88TQGN86IwK77wTWluUS/0FuPu6WNDQxm\n"
"xVuG/G5U6eOubnwypjOqO19Tz9BIkgDDM1FuUrysWjcxQR9owlpb6i/ibAE6uANK\n"
"N7dkUOZPzGKx+xnfarhkWmrzQwm4zTIHwjYfiy4e38n5GtwWBsf/wog3tAiv6X21\n"
"RwIDAQAB\n"
	"-----END PUBLIC KEY-----\n";
static const char PRIVKEY[] =
"-----BEGIN RSA PRIVATE KEY-----\n"
"Proc-Type: 4,ENCRYPTED\n"
"DEK-Info: AES-256-CBC,DE8B79957FCB45AC0EAA83FE849CF70B\n"
"\n"
"v/Fy60UiJMfaTmA2gv25yyFaV3Aq4jApE05PfiuIs7d1TQJMF+8d+ysIk5GsKIuE\n"
"KjeK+us+sS4VcunG7jccbSk4ESo0kEVH4Xm1/Fb4oJ9N1xB9a33bgXxmxts/vMHo\n"
"RHO0g3z9Se/FD7gh0ooV5A2p5eHxoGh9DESg+3L+vG/aouIwgmGxRcH8/aU1/sXG\n"
"0AcNygkqMbsHeZrRLAb23hfWfTY1blqOB5R+mGvkLW8ij3qS32Lw4IEYx7BxkvDg\n"
"ref/ECogM7cALrZ61RjXd0KWzbGUfwVEj2RiPusoAEbTzCITH41aNm27jrHvxEYK\n"
"+CJl9strNyXb5gpXiMwySAFEyXfYBcKqxbo5+273MDrph9yXLLQisS5yzcUeFGyj\n"
"d3n3t5MUsbehbI8nNqIl4u8A6i8NftaMwJoN/MFb3mXcLppczCI0TbTYf7RAXs1k\n"
"jZWvVDhVCrF+dvNkKtEuytG/oMgqfVvWZ+Z75nvcgu8HP7X3WZf3VLPfcJ1+hT/m\n"
"o0szfeghKDH85zASDE+0zARj0bz5bS6fiYjd8/+aj49UYLkc29052U82bNCOAMV7\n"
"qk87KoxuNkWkGnXYwK9TreETXaJ5h0CUYMFDAp+CHcBxnztGpAr4R3OgxYHZE6vL\n"
"B//kaV7aRS3LR+qiKAITFctR9NxnBOTO53pPdjrFVmEx5G2Xbmjd1CKdqOOUCLr3\n"
"4tkwGQwToEZsFQjPdV5WQsaY+tWGB+80Pphghs5cJSIekiYS9OzmG89jiBtMsnPJ\n"
"gmTxq7xuInA6x+SQqot3PSlpCJd2a2C9hFVlNR3cGhpMTEDzFsaxrRSjAjvUo1cq\n"
"qtI3usLLYyvaN2SxfmFV0gte5DmfnnY+zpn6rFJG/9js9eLtzdiuWZ9es1i/AGHt\n"
"aa/Ag4QT0Ot1C7isoRmUOdTgjNaAcU8duPcYsES3WkqhPruLyNeDxp4Rb0yqMs8e\n"
"+pJJ9pnZMGQZtNNHHdh2K9ZrLAC1NhktXLUqLPusZ/TwPcMViyiKzTtR5qMHNPxd\n"
"fsm4kjrFdmjwHbSJQSt5cwo8w7mgi0zPdF2drh6XPyd9owzgs8nXc3Jml6X80B/i\n"
"XqvXdmXx9g/YOrp3Dl60Y/7+H6hKkcyu35yWhig3ek+sErKrwK4SJZCCWuG1WQnA\n"
"R9Dlwy45JQ50Cjrezu/KQDMrImRvtfwnxBbZs7Ho6+WtT2d+3dwbJhX2y8U1fFN1\n"
"DbfTfVjJX54iJZUxIyRnB0cM3nkB+oCmxurHpfeIi6o4ciwpAwnlCN/8jNGl4T1q\n"
"VXnQ+iykXyiZtITzw+5Ni2rYQeO2lmPy3vAWI74ncP606qW1IRLzJvyHP0YcD/gd\n"
"23SdO2Ld10f/gUb6GuAkBBl4CbBVeMpuxHtP3AjckLt8nTvhHHs/k8CajY7KVMHr\n"
"jtmQllIJd9NfTSKvGfJQkip1KoclyvCi/+4gOTpuF9n/aXdzrDpdHhzS4qkISlg3\n"
"lOdan2tnmhaNuN8cETzWnojWmKwqVMqFxQ02/tTc854hgx+L8HR1bIUQasZrwB1m\n"
"jttpXwjarCunEhYDI9mBjFN+76CNmmj5DLm8sAe5JxamViw/J3xS9vpzV9kTtMk+\n"
"-----END RSA PRIVATE KEY-----\n";

static const char CC1_PUBKEY[] =
"-----BEGIN PUBLIC KEY-----\n"
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAqRgQSU2l7ExLLFQio3tB\n"
"26XjdJ0Bz678CduPOka4sfGWTXCZIaFGzRlBf9lnIIZJKXS5qre+BMYXZYTVQ7tz\n"
"X726lHe6p6+4vR2y2pr2JZTPtRi9RIumIEo23eAj5Fu9vF6oxIeLHGtLy9QGhYyS\n"
"lKsj6oqwT1yHjRAg88R8daic10D0Vl0yz9Kytrtw+dHXUQ395klzYUrn7GwZbUey\n"
"JFl+t3JmrgCEYfOosyXuT8eWhTLGzyatng+dQdHWnuRIrwQ+doLOrjBmGShV2bqv\n"
"E5DkDq7cXV1ij/pVCoSkRPn7M9JJM7EBXSvJUZuncQEJJuFkpkyPZ9MrqaiZqHCf\n"
"/wIDAQAB\n"
"-----END PUBLIC KEY-----\n";

static const char CP1_PUBKEY[] =
"-----BEGIN PUBLIC KEY-----\n"
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAoOwvzqeX7WrV9QWuBYBW\n"
"hODD1zBdCCGMKw5d4SfY0DWxzjoWOAVxMImfgkO8Dky1bz0W/QER1aMXs8xqCTEh\n"
"M7HnQ3qo2jTZnnaYmYLAJcbooBmKWj5tS3OsjKrsJphLF/qv+JQYj/PrjMMqJh5c\n"
"V/1+5h8NPo8V6wdoAsCmWQeJdzOJZu2Ld86nffa0wXQONswZrpPkEiUT5yhPQs8Z\n"
"fGN+8nxCK6fl7nSwv6578NsreONn9ZeJUayj6uR7AqQwuIYNQtdcd+N2IGgRho6m\n"
"sfPxf791JJiDXVtDnzPTzSLdIdMItgLKy744I8ai2Q8pt1vFPontkgee/PKagz4O\n"
"XQIDAQAB\n"
"-----END PUBLIC KEY-----\n";

#endif // KEYDEFINE_H
