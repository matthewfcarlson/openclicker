from io import TextIOWrapper
import sys
import os
import datetime
import enum
import json
from pathlib import Path

MIN_PRESENTER_MESSAGE_ID = 20

class PresenterMessageId(enum.IntEnum):
    def _generate_next_value_(name, start, count, last_values):
        """generate consecutive automatic numbers starting from zero"""
        return count + MIN_PRESENTER_MESSAGE_ID
    # These are the message ids for the presenter
    # The first message id is MIN_PRESENTER_MESSAGE_ID
    RemoteButtonPressed = enum.auto(),
    BridgeButtonPressed = enum.auto(),
    RemoteHeartBeat = enum.auto(),
    RemoteRequestState = enum.auto(),
    PresenterSetState = enum.auto(),
    PresenterRemotePacket = enum.auto(),

# these are functions that presenter will never send out
IGNORED_PRESENTER_OUT_FUNCTIONS = [
    PresenterMessageId.PresenterRemotePacket.name,
    PresenterMessageId.RemoteButtonPressed.name,
    PresenterMessageId.BridgeButtonPressed.name,
    PresenterMessageId.RemoteHeartBeat.name,
    PresenterMessageId.RemoteRequestState.name,
]

class PresenterLittleStates(enum.StrEnum):
    # these are the little states that the presenter can tell remotes to be in
    DarkState = 'dark',
    MultipleChoiceState = 'mc',
    EmojiState = 'emoji',

class Definitions:
    uint8_t = ['uint8_t','z.number().int().nonnegative().lte(255)', '8bit unsigned integer']
    uint8_t_button = ['uint8_t','z.number().int().nonnegative().lt(4)', '8bit unsigned integer between 0 and 4']
    uint8_t_battery = ['uint8_t','z.number().int().nonnegative().lt(100)', '8bit unsigned integer between 0 and 100']
    uint16_t = ['uint16_t','z.number().int().nonnegative().lte(65535)', '16bit unsigned integer']
    uint32_t = ['uint32_t','z.number().int().nonnegative().lte(0xFFFFFFFF)', '32bit unsigned integer']
    char32_little_state = ['char[20]','LittleStateNamesZ', '20 character string that is a little state name']
    char64 = ['char[64]','z.string().max(63)', '64 character (including null terminator) string']
    mac_address = ['uint8_t[6]','z.string().length(17)', 'A 6 byte mac address that is encoded in and out of a string for ease'] # 05:fc:4f:d7:11:a5

STRUCTS = {
    PresenterMessageId.RemoteButtonPressed.value: {
        'button_id': Definitions.uint8_t_button,
    },
    PresenterMessageId.BridgeButtonPressed.value: {
        'button_id': Definitions.uint8_t_button,
    },
    PresenterMessageId.RemoteHeartBeat.value: {
        'battery_level': Definitions.uint8_t_battery,
        'state_name': Definitions.char32_little_state,
    },
    PresenterMessageId.RemoteRequestState.value: {
        'state_hash1': Definitions.uint32_t,
        'state_hash2': Definitions.uint32_t,
        'state_hash3': Definitions.uint32_t,
        'state_hash4': Definitions.uint32_t,
    },
    PresenterMessageId.PresenterRemotePacket.value: {
        'from_mac': Definitions.mac_address,
        # this one is special since the remaining data is the rest of the packet that is forwarded
    },
    PresenterMessageId.PresenterSetState.value: {
        'state_name': Definitions.char32_little_state,
    },
    PresenterLittleStates.DarkState.value: {
        'state_name': Definitions.char32_little_state,
    },
    PresenterLittleStates.EmojiState.value: {
        'emoji_choice_1': Definitions.uint32_t,
        'emoji_choice_2': Definitions.uint32_t,
        'emoji_choice_3': Definitions.uint32_t,
        'emoji_choice_4': Definitions.uint32_t,
    },
     PresenterLittleStates.MultipleChoiceState.value: {
        'num_choices': Definitions.uint8_t,
        'question_text': Definitions.char64,
    },
}

C_TYPE_TO_PRINT = {
    'uint8_t': 'u',
    'uint8_t[6]': 's',
    'uint16_t': 'u',
    'uint32_t': 'u',
    'char[32]': 's',
    'char[64]': 's',
    'char[20]': 's',
}

def generate_message_structs():
    for message_id in PresenterMessageId:
        base_literal_fields = {
            'id': ['uint8_t', f"z.literal(PresenterMessageId.{message_id.name})", f"{message_id.name}", message_id.value],
        }
        if message_id == PresenterMessageId.PresenterSetState:
            for little_state in PresenterLittleStates:
                base_fields = STRUCTS.get(message_id.value, {})
                extended_fields = STRUCTS.get(little_state.value, {})
                fields = base_fields.copy()
                fields.update(extended_fields)
                literal_fields = base_literal_fields.copy()
                literal_fields['state_name'] = fields['state_name'].copy()
                del fields['state_name']
                literal_fields['state_name'][1] = f"z.literal(LittleStateNames.{little_state.name})"
                literal_fields['state_name'][2] = f'"{little_state.value}"'
                yield {
                    "name": f"{message_id.name}{little_state.name}",
                    'fields': fields,
                    "literal_fields": literal_fields
                }
        yield {
            "name": message_id.name,
            'fields': STRUCTS.get(message_id.value, {}),
            'literal_fields': base_literal_fields
        }



class CommonFileGenBase:
    def __init__(self, fp:TextIOWrapper):
        self.fp = fp

        self.write_common_header()
        self.write_message_type_enum(PresenterMessageId, MIN_PRESENTER_MESSAGE_ID)
        self.write_little_state_enum(PresenterLittleStates)
        self.write_message_structs(generate_message_structs())
        self.write_parsers(generate_message_structs())

    def write_common_header(self):
        self.fp.writelines([
            '/**\n',
            f' AUTOGENERATED ON {datetime.datetime.now()}\n',
            ' DO NOT MODIFY THIS FILE MANUALLY\n',
            ' IT WILL BE OVERWRITTEN ON THE NEXT BUILD\n',
            ' Use the generate.py script in the protocol folder to regenerate this file\n',
            '**/\n\n',
        ])
    def write_message_type_enum(self, ids, id_offset):
        pass
    def write_little_state_enum(self, little_states):
        pass
    def write_message_structs(self, message_struct_generator):
        pass
    def write_parsers(self, message_struct_generator):
        pass

    def get_struct_name(self, name):
        return f"Protocol{name}"

class TypescriptFileGen(CommonFileGenBase):
    def write_common_header(self):
        super().write_common_header()
        self.fp.write("import {z} from 'zod';\n\n")

    def write_message_type_enum(self, message_ids, id_offset):
        self.fp.write(f"export const MIN_PRESENTER_MESSAGE_ID = {id_offset};\n")
        self.fp.write("export enum PresenterMessageId {\n")
        for message_id in message_ids:
            self.fp.write(f"    {message_id.name} = {message_id.value},\n")
        self.fp.write("}\n")
        self.fp.write("const PresenterMessageIdZ = z.nativeEnum(PresenterMessageId);\n\n")

    def write_little_state_enum(self, little_states):
        self.fp.write("export enum LittleStateNames {\n")
        for little_state in little_states:
            self.fp.write(f"    {little_state.name} = '{little_state.value}',\n")
        self.fp.write("}\n")
        self.fp.write("const LittleStateNamesZ = z.nativeEnum(LittleStateNames);\n\n")

    def write_message_structs(self, message_struct_generator):
        self.fp.write("const PresenterMessageBaseZ = z.object({id: PresenterMessageIdZ});\n")
        emitted_struct_names = set()
        for message_struct in message_struct_generator:
            struct_name = self.get_struct_name(message_struct['name'])
            self.fp.write(f'export const {struct_name}Z = PresenterMessageBaseZ.extend({{\n')
            all_fields = message_struct['literal_fields'].copy()
            all_fields.update(message_struct['fields'])
            for field_name, field in all_fields.items():
                _, zod_type, *comment = field
                self.fp.write(f"    {field_name}: {zod_type}, // {comment[0]} \n")
            self.fp.write("});\n")
            emitted_struct_names.add(struct_name)
            self.fp.write(f"export type {struct_name} = z.infer<typeof {struct_name}Z>;\n")
            self.fp.write('\n')
        self.fp.write("export type PresenterMessage = ")
        self.fp.write(" | ".join(sorted(emitted_struct_names)))
        self.fp.write(";\n\n")

    def write_parsers(self, message_struct_generator):
        messages_structs = list(message_struct_generator)
        self.fp.write("export function parsePresenterMessage(data: object, warn=false) {\n")
        for message_struct in messages_structs:
            struct_name = self.get_struct_name(message_struct['name'])
            self.fp.write("    try {\n")
            self.fp.write(f"        return {struct_name}Z.parse(data);\n")
            self.fp.write("    } catch(e) { if (warn) console.warn(e)}\n\n")
        self.fp.write("}\n")

        self.fp.write("export const MessageCreators = {\n")
        for message_struct in messages_structs:
            if message_struct['name'] in IGNORED_PRESENTER_OUT_FUNCTIONS:
                continue
            struct_name = self.get_struct_name(message_struct['name'])
            self.fp.write(f"  {message_struct['name']}(\n")
            fields = ["   wasm: any", "   to_mac: string"]
            field_names = []
            for field_name, field in message_struct['fields'].items():
                field_names.append(field_name)
                fields.append(f"   {field_name}: {struct_name}['{field_name}']")
            self.fp.write(",\n".join(fields))
            self.fp.write("\n  ) {\n")
            self.fp.write("    let func_name = 'create_"+message_struct['name']+"';\n")
            #  wasm.ccall(
            # look at C_TYPES_TO_PRINT to determine the type
            types = ["'string'" if C_TYPE_TO_PRINT[message_struct['fields'][field][0]] == 's' else "'number'" for field in field_names] 
            self.fp.write(f"    return wasm.ccall(func_name, 'string', [{",".join(['"string"',]+types)}], [{",".join(["to_mac",]+field_names)}]);\n")
            self.fp.write("  },\n")
        self.fp.write("};\n")

class CHeaderFileGen(CommonFileGenBase):
    def write_common_header(self):
        super().write_common_header()
        self.fp.write("#pragma once\n\n")
        self.fp.write("#pragma pack(push, 1)\n")
        self.fp.write("#ifndef __PRESENTER_PROTOCOL_H__\n")
        self.fp.write("#define __PRESENTER_PROTOCOL_H__\n\n")
        for include in ["stdint.h", "stdlib.h", "stdio.h", "strings.h"]:
            self.fp.write(f"#include <{include}>\n")
        self.fp.write("\n")


    def get_struct_name(self, name):
        return f"PresenterProtocol{name}"

    def write_message_type_enum(self, message_ids, id_offset):
        self.fp.write(f"#define MIN_PRESENTER_MESSAGE_ID {id_offset}\n")
        self.fp.write("enum PresenterMessageId {\n")
        for message_id in message_ids:
            self.fp.write(f"    {message_id.name} = {message_id.value},\n")
        self.fp.write("};\n\n")

    def write_little_state_enum(self, little_states):
        for little_state in little_states:
            name = little_state.name.upper()
            self.fp.write(f"#define LITTE_STATE_{name}_NAME \"{little_state.value}\"\n")
            self.fp.write(f"#define LITTE_STATE_{name}_LENGTH {len(little_state.value)}\n")
        self.fp.write("\n")

    def write_message_structs(self, message_struct_generator):
        for message_struct in message_struct_generator:
            struct_name = self.get_struct_name(message_struct['name'])+"_t"
            self.fp.write("typedef struct {\n")
            all_fields = message_struct['literal_fields'].copy()
            all_fields.update(message_struct['fields'])
            for field_name, field in all_fields.items():
                c_type, _, *comment = field
                field_end = ""
                if c_type.endswith("]"):
                    c_type, field_end = c_type.split("[")
                    field_end = f"[{field_end}"
                self.fp.write(f"    {c_type} {field_name}{field_end}; // {comment[0]}\n")
            self.fp.write(f"}} {struct_name};\n")
            self.fp.write("\n")

            fields = message_struct['fields']
            macro_args_list = ",".join([f"_{i}" for i in range(len(fields)+1)])
            self.fp.write(f"#define PRESENTER_{message_struct['name'].upper()}({macro_args_list}) \\\n")
            self.fp.write(f"    {struct_name} _0 = {{ \\\n")
            for field_name, field in message_struct['literal_fields'].items():
                self.fp.write(f"        .{field_name} = {field[2]}, \\\n")
            memcpy_fields = False
            for i, field in enumerate(fields):
                field_type = fields[field][0]
                if field_type.endswith("]"):
                    memcpy_fields = True
                    continue
                self.fp.write(f"        .{field} = _{i+1}, \\\n")
            self.fp.write("    }")
            if memcpy_fields:
                self.fp.write("")
                for i, field in enumerate(fields):
                    field_type = fields[field][0]
                    if not field_type.endswith("]"):
                        continue
                    self.fp.write(f"; \\\n   memcpy(_0.{field}, _{i+1}, sizeof(_0.{field}))")
            self.fp.write(";\n")
            self.fp.write("\n")

    def write_parsers(self, message_struct_generator):
        self.fp.write("#endif\n")
        self.fp.write("#pragma pack(pop)\n")


class CHelpersFileGen(CommonFileGenBase):

    def get_struct_name(self, name):
        return f"PresenterProtocol{name}"

    def write_common_header(self):
        super().write_common_header()
        for include in ["protocol/presenter_protocol.h", "string.h", "emscripten.h", "bridge/bridge.hpp"]:
            self.fp.write(f"#include <{include}>\n")
        self.fp.write("\n")
        self.fp.write("const uint32_t msg_text_size = 255;\n")
        self.fp.write("BridgeTransport* transport = new BridgeTransport();\n")
        self.fp.write("const uint8_t bridge_mac[6] = {0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF};\n")
        self.fp.write("\n")

    def write_parsers(self, message_struct_generator):
        message_structs = list(message_struct_generator)
        def generate_condition(var:str, field) -> str:
            c_type = C_TYPE_TO_PRINT.get(field[0], "d")
            if c_type == 's':
                return f"strncmp({var},{field[2]}, {len(field[2].strip('"'))}) == 0"
            return f"{var} == {field[2]}"

        def generate_matching(message_struct):
            conditions = []
            for index, field in enumerate(message_struct['literal_fields'].items()):
                field_name, field_type = field
                if index == 0:
                    condition = generate_condition("message[0]", field_type)
                else:
                    struct_name = self.get_struct_name(message_struct['name'])
                    temp_val = f"(({struct_name}_t*)message)"
                    condition = generate_condition(f"{temp_val}->{field_name}", field_type)
                conditions.append(condition)
            if len(condition) == 0:
                self.fp.write(f"    if (1) {{\n")
            elif len(condition) == 1:
                self.fp.write(f"    if ({condition}) {{\n")
            else:
                condition = " && ".join([f"({x})" for x in conditions])
                self.fp.write(f"    if ({condition}) {{\n")

        def generate_body(sprintf_fmt, sprintf_args_list: list):
            sprintf_args = ",".join(sprintf_args_list)
            if len(sprintf_args) > 0:
                sprintf_args = ","+sprintf_args
            self.fp.write(f"        int size_needed = snprintf(NULL, 0, \"{sprintf_fmt}\"{sprintf_args}) + 1;\n")
            self.fp.write(f"        char* json = (char*)malloc(size_needed);\n")
            self.fp.write(f"        if (json == NULL) return NULL;\n")
            self.fp.write(f"        bzero(json, size_needed);\n")
            self.fp.write(f"        snprintf(json, size_needed, \"{sprintf_fmt}\"{sprintf_args});\n")
            self.fp.write(f"        return json;\n")

        def generate_sprintf(message_struct):
            fmt = []
            args = []
            def get_items():
                for field_name, field in message_struct['literal_fields'].items():
                    format_str = str(field[-1]).replace('"', '\\"')
                    yield field_name, format_str, None
                for field_name, field in message_struct['fields'].items():
                    if field[0] in C_TYPE_TO_PRINT:
                        format_type = C_TYPE_TO_PRINT[field[0]]
                        
                        if format_type == 's':
                            format_str = f"\\\"%s\\\""
                        else:
                            format_str = f"%{format_type}"
                        yield field_name, format_str, f"msg->{field_name}"
                    elif field[0].endswith("]"):
                        # we need to yield it as a bytes stream
                        pass
                    else:
                        raise NotImplementedError(f"Unknown type {field[0]}")

            for field_name, format_str, arg in get_items():
                if arg:
                    args.append(arg)
                fmt.append(f'\\\"{field_name}\\\":{format_str}')

            return "{"+",".join(fmt)+"}", args

        # start with mac address wrapped functions
        self.fp.write('char* presenter_message_to_json(const uint8_t* message, const uint32_t message_size, char* from_mac, char* to_mac) {\n')
        for message_struct in message_structs.copy():
            generate_matching(message_struct)
            self.fp.write(f"        {self.get_struct_name(message_struct['name'])}_t* msg = ({self.get_struct_name(message_struct['name'])}_t*)message;\n")
            fmt, args = generate_sprintf(message_struct)
            generate_body('{\\"from\\":\\"%s\\",\\"to\\":\\"%s\\",\\"msg\\":'+fmt+"}", ['from_mac','to_mac']+args)
            self.fp.write("    }\n")
            self.fp.write("\n")
        self.fp.write("   return NULL;\n}\n")
        self.fp.write("\n")

        self.fp.write('char* presenter_data_to_json(const uint8_t* message, const uint32_t message_size) {\n')
        for message_struct in message_structs:
            generate_matching(message_struct)
            self.fp.write(f"        {self.get_struct_name(message_struct['name'])}_t* msg = ({self.get_struct_name(message_struct['name'])}_t*)message;\n")
            fmt, args = generate_sprintf(message_struct)
            generate_body(fmt, args)
            self.fp.write("    }\n")
            self.fp.write("\n")
        self.fp.write("   return NULL;\n}\n")
        self.fp.write("\n")

        # now start with functions that generate the message
        for message_struct in message_structs:
            self.fp.write(f"extern \"C\" char* create_{message_struct['name']}(\n")
            params = ["char* mac_addr_str"]
            fields = message_struct['fields']
            for field_name, field in fields.items():
                field_c_type = field[0]
                if field_c_type.endswith("]"):
                    field_c_type = field[0].split("[")[0]+"*"
                params.append(f"{field_c_type} {field_name}")
            for index, param in enumerate(params):
                if index != 0:
                    self.fp.write(",\n")
                self.fp.write(f"    {param}")
            self.fp.write("\n) {\n")
            # now we need to generate the message
            keys = ["msg",] + list(fields.keys())
            self.fp.write(f"    PRESENTER_{message_struct['name'].upper()}({', '.join(keys)});\n")
            self.fp.write("    uint8_t mac_addr[8];\n")
            self.fp.write("    uint32_t mac_addr_str_len = strnlen(mac_addr_str, 22);\n")
            self.fp.write("    transport->ReadMacFromString(mac_addr_str, &mac_addr_str_len, mac_addr);\n")
            self.fp.write("    char* msg_txt = (char*)malloc(msg_text_size);\n")
            self.fp.write("    if (msg_txt == NULL) return nullptr;\n")
            self.fp.write("    bzero(msg_txt, msg_text_size);\n")
            self.fp.write("    transport->ConvertMessageToString(bridge_mac, mac_addr, (uint8_t*)&msg, sizeof(msg), msg_txt, msg_text_size);\n")
            self.fp.write("    return msg_txt;\n")
            self.fp.write("}\n")

class MakefileFileGen(CommonFileGenBase):
    def write_common_header(self):
        self.fp.write(f"# AUTOGENERATED ON {datetime.datetime.now()}\n")
        pass

    def write_parsers(self, message_struct_generator):
        self.fp.write("PRESENTER_EXPORTED_FUNCTIONS = ")
        functions = ['_message_string_to_json', "_message_json_to_string", "_generate_base64_little_state_hash_json"]
        for message_struct in message_struct_generator:
            functions.append(f"_create_{message_struct['name']}")
        self.fp.write(",".join([f'"{x}"' for x in functions]))

def main():
    # first we need to find the path of the folder this file is in
    # then we need to find the path of the folder above this folder
    root_dir = Path(__file__).resolve().parent.parent
    # get the path to presenter/src/common/Protocol.ts relative to the root dir
    protocol_path = root_dir / "presenter" / "src" / "common" / "Protocol.ts"
    # now get the path to the firmware/lib/OpenClicker/include/protocol/presenter_protocol.h relative to the root dir
    firmware_path = root_dir / "firmware" / "lib" / "OpenClicker" / "include" / "protocol" / "presenter_protocol.h"
    # now get the path to emscripten file
    emscripten_path = root_dir / "firmware" / "lib" / "OpenClicker" / "web" / "helpers.hpp"

    # now we need to generate the typescript
    protocol_path.unlink(missing_ok=True)
    with open(protocol_path, "w") as fp:
        TypescriptFileGen(fp)

    # generate c header
    firmware_path.unlink(missing_ok=True)
    with open(firmware_path, "w") as fp:
        CHeaderFileGen(fp)

    # generate c helpers
    emscripten_path.unlink(missing_ok=True)
    with open(emscripten_path, "w") as fp:
        CHelpersFileGen(fp)

    makefile_path = root_dir / "firmware" / "lib" / "OpenClicker" / "web" / "generated.mk"
    makefile_path.unlink(missing_ok=True)
    with open(makefile_path, "w") as fp:
        MakefileFileGen(fp)

    return 0


if __name__ == "__main__":
    sys.exit(main())