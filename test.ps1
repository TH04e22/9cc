function test-case() {
    $expected = $args[0]
    $input = $args[1]

    ./9cc $input | Out-File 'tmp.s' -Encoding ascii

    gcc -o tmp tmp.s
    ./tmp
    $actual = $LastExitCode

    if( $actual -eq $expected ) {
        Write-Output "$input => $actual"
    } else {
        Write-Output "$input = > $expected expected, but got $actual"
        exit 1
    }
}

make
test-case 63 'a = 76; b = 60; c = 54; d = (a + b + c)/3;'
test-case 82 'var1 = 12; var2 = 70; var3 = var1 + var2; return var3;'
test-case 16 '_temp1 = 24 - 5 * 4; tem_4 = 12/3; num_tem = _temp1 + tem_4; return _temp1 + tem_4 + num_tem;'

Write-Output OK
