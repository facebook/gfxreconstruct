#!/usr/bin/env python3
#
# Copyright (c) 2021 LunarG, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.

from base_generator import *
from dx12_base_generator import *
from dx12_decoder_header_generator import DX12DecoderHeaderGenerator
from base_struct_decoders_body_generator import *
from base_decoder_body_generator import *


# Generates C++ functions responsible for decoding DX12 API calls
class DX12DecoderBodyGenerator(
        DX12DecoderHeaderGenerator,
        BaseStructDecodersBodyGenerator, BaseDecoderBodyGenerator):

    # Method override
    def write_include(self):
        code = ("\n"
                "#include \"generated_dx12_decoder.h\"\n"
                "#include \"generated_dx12_struct_decoders_forward.h\"\n"
                "#include \"decode/custom_dx12_struct_decoders_forward.h\"\n"
                "\n")
        write(code, file=self.outFile)

    # Met#include "util/defines.h"hod override
    def generateFeature(self):
        self.cmdNames = []
        self.methodNames = []
        DX12BaseGenerator.generateFeature(self)
        self.write_function_call()
        BaseDecoderBodyGenerator.generate_feature(self, 'DX12')
        self.newline()
        self.generate_dx12_method_feature()

    def generate_dx12_method_feature(self):
        first = True
        for method in self.getFilteredMethodNames():
            self.methodNames.append(method)

            info = self.featureMethodParams[method]
            return_type = info[0]
            values = info[2]

            cmddef = '' if first else '\n'
            cmddef += ('size_t DX12Decoder::Decode_{}(format::HandleId object_id, const uint8_t* parameter_buffer, size_t buffer_size)\n'  # noqa
                       .format(method))
            cmddef += '{\n'
            cmddef += '    size_t bytes_read = 0;\n'
            cmddef += '\n'
            cmddef += self.makeCmdBody(return_type, method, values, True)
            cmddef += '\n'
            cmddef += '    return bytes_read;\n'
            cmddef += '}'
            write(cmddef, file=self.outFile)
            first = False

    def write_function_call(self):
        code = ("void DX12Decoder::DecodeFunctionCall(format::ApiCallId  call_id,\n"  # noqa
                "                                     const ApiCallInfo& call_info,\n"  # noqa
                "                                     const uint8_t*     parameter_buffer,\n"  # noqa
                "                                     size_t             buffer_size){}\n"  # noqa
                "\n"
                "void DX12Decoder::DecodeMethodCall(format::ApiCallId  call_id,\n"  # noqa
                "                                   format::HandleId   object_id,\n"  # noqa
                "                                   const ApiCallInfo& call_info,\n"  # noqa
                "                                   const uint8_t*     parameter_buffer,\n"  # noqa
                "                                   size_t             buffer_size) {}\n"  # noqa
                .format(self.get_decode_function_call_body(),
                        self.get_decode_method_call_body()))
        write(code, file=self.outFile)

    # Method override
    def get_decode_function_call_body(self):
        code = '\n'\
               '{\n'\
               '    GFXRECON_UNREFERENCED_PARAMETER(call_info);\n'\
               '    switch (call_id)\n'\
               '    {\n'

        header_dict = self.source_dict['header_dict']
        for k, v in header_dict.items():
            for m in v.functions:
                if self.is_required_function_data(m):
                    code += ("    case format::ApiCallId::ApiCall_{0}:\n"
                             "        Decode_{0}(parameter_buffer, buffer_size);\n"  # noqa
                             "        break;\n".format(m['name']))

        code += 'default:\n'\
                '    break;\n'\
                '    }\n'\
                '}\n'
        return code

    # Method override
    def get_decode_method_call_body(self):
        code = '\n'\
               '{\n'\
               '    GFXRECON_UNREFERENCED_PARAMETER(call_info);\n'\
               '    switch (call_id)\n'\
               '    {\n'

        header_dict = self.source_dict['header_dict']
        for k, v in header_dict.items():
            for k2, v2 in v.classes.items():
                if self.is_required_class_data(v2):
                    for m in v2['methods']['public']:
                        code += ("    case format::ApiCallId::ApiCall_{0}_{1}:\n"  # noqa
                                 "        Decode_{0}_{1}(object_id, parameter_buffer, buffer_size);\n"  # noqa
                                 "        break;\n".format(k2, m['name']))

        code += 'default:\n'\
                '    break;\n'\
                '    }\n'\
                '}\n'
        return code