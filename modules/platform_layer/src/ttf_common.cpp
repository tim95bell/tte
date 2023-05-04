
#pragma once

#include "../include/tte/platform_layer/platform_layer.hpp"
#include "../include/tte/platform_layer/ttf.hpp"
#include <tte/common/assert.hpp>
#include <cstdlib>
#include <cstring>

namespace tte { namespace ttf {
    namespace required_tables_bitset {
        static const constexpr U16 cmap = 1 << 0;
        static const constexpr U16 glyf = 1 << 1;
        static const constexpr U16 head = 1 << 2;
        static const constexpr U16 hhea = 1 << 3;
        static const constexpr U16 hmtx = 1 << 4;
        static const constexpr U16 loca = 1 << 5;
        static const constexpr U16 maxp = 1 << 6;
        static const constexpr U16 name = 1 << 7;
        static const constexpr U16 post = 1 << 8;
    }

    struct OffsetSubtable {
        // 'true' (0x74727565) or 0x00010000 => true type font
        U32 scaler_type;
        // number of tables (excluding table directory itself and any subtables)
        U16 num_tables;
        // (maximum power of 2 <= num_tables) * 16
        // ((2**floor(log2(num_tables))) * 16
        U16 search_range;
        // log2(maximum power of 2 <= num_tables)
        // log2(search_range / 16) == floor(log2(num_tables))
        U16 entry_selector;
        // (num_tables * 16) - search_range
        U16 range_shift;
    };

    static const constexpr Length offset_subtable_size = sizeof(U32) + (sizeof(U16) * 4);
    static const constexpr Length num_required_tables = 9;
    static const constexpr Length table_directory_size = sizeof(U32) * 4;

    [[nodiscard]] bool read_offset_subtable_unchecked(platform_layer::FileIter* file_iter, OffsetSubtable* result) {
        platform_layer::read_big_endian_and_move_unchecked(file_iter, &result->scaler_type);
        if (result->scaler_type != 0x74727565 && result->scaler_type != 0x00010000) {
            return false;
        }

        platform_layer::read_big_endian_and_move_unchecked(file_iter, &result->num_tables);
        if (result->num_tables < num_required_tables) {
            return false;
        }

        platform_layer::read_big_endian_and_move_unchecked(file_iter, &result->search_range);
        platform_layer::read_big_endian_and_move_unchecked(file_iter, &result->entry_selector);
        platform_layer::read_big_endian_and_move_unchecked(file_iter, &result->range_shift);

        return true;
    }

    struct TableDirectory {
        U32 tag;
        // the unsigned sum of the longs (U32) in a table
        U32 check_sum;
        // offset from beginning of sfnt (Spline Font or Scalable Font; ttf file format, containing PostScript, OpenType, or TrueType fonts)
        U32 offset;
        // length of this table in byte (actual length not padded length)
        U32 size;
    };

    [[nodiscard]] bool read_table_directory_unchecked(platform_layer::FileIter* file_iter, TableDirectory* table_directory) {
        platform_layer::read_big_endian_and_move_unchecked(file_iter, &table_directory->tag);
        // TODO(TB): check 'check_sum' is valid
        platform_layer::read_big_endian_and_move_unchecked(file_iter, &table_directory->check_sum);
        platform_layer::read_big_endian_and_move_unchecked(file_iter, &table_directory->offset);
        platform_layer::read_big_endian_and_move_unchecked(file_iter, &table_directory->size);

        return platform_layer::has_enough_space_from_begin(file_iter, table_directory->offset, table_directory->size);
    }

    struct FontDirectory {
        OffsetSubtable offset_subtable;
        TableDirectory* table_directory_array;
    };

    void free_font_directory(FontDirectory* font_directory) {
        if (font_directory->table_directory_array) {
            free(font_directory->table_directory_array);
            font_directory->table_directory_array = nullptr;
        }
    }

    [[nodiscard]] bool read_font_directory(platform_layer::FileIter* file_iter, FontDirectory* font_directory) {
        // check enough space is left in file_iter for offset subtable
        if (!platform_layer::has_enough_space_from_at(file_iter, offset_subtable_size)) {
            return false;
        }

        if (!read_offset_subtable_unchecked(file_iter, &font_directory->offset_subtable)) {
            return false;
        }

        // check enough space is left in file_iter for table directory array
        if (!platform_layer::has_enough_space_from_at(file_iter, font_directory->offset_subtable.num_tables * table_directory_size)) {
            return false;
        }

        font_directory->table_directory_array = static_cast<TableDirectory*>(malloc(sizeof(TableDirectory) * font_directory->offset_subtable.num_tables));
        memset(font_directory->table_directory_array, 0, sizeof(TableDirectory) * font_directory->offset_subtable.num_tables);
        U16 required_tables_bitset = 0;
        for (Length i = 0; i < font_directory->offset_subtable.num_tables; ++i) {
            if (!read_table_directory_unchecked(file_iter, font_directory->table_directory_array + i)) {
                return false;
            }

            switch (static_cast<TableDirectory_Tag>(font_directory->table_directory_array[i].tag)) {
                case TableDirectory_Tag::CMAP: {
                    required_tables_bitset |= required_tables_bitset::cmap;
                } break;
                case TableDirectory_Tag::GLYF: {
                    required_tables_bitset |= required_tables_bitset::glyf;
                } break;
                case TableDirectory_Tag::HEAD: {
                    required_tables_bitset |= required_tables_bitset::head;
                } break;
                case TableDirectory_Tag::HHEA: {
                    required_tables_bitset |= required_tables_bitset::hhea;
                } break;
                case TableDirectory_Tag::HMTX: {
                    required_tables_bitset |= required_tables_bitset::hmtx;
                } break;
                case TableDirectory_Tag::LOCA: {
                    required_tables_bitset |= required_tables_bitset::loca;
                } break;
                case TableDirectory_Tag::MAXP: {
                    required_tables_bitset |= required_tables_bitset::maxp;
                } break;
                case TableDirectory_Tag::NAME: {
                    required_tables_bitset |= required_tables_bitset::name;
                } break;
                case TableDirectory_Tag::POST: {
                    required_tables_bitset |= required_tables_bitset::post;
                } break;
            }
        }

        if (required_tables_bitset & required_tables_bitset::cmap &&
            required_tables_bitset & required_tables_bitset::glyf &&
            required_tables_bitset & required_tables_bitset::head &&
            required_tables_bitset & required_tables_bitset::hhea &&
            required_tables_bitset & required_tables_bitset::hmtx &&
            required_tables_bitset & required_tables_bitset::loca &&
            required_tables_bitset & required_tables_bitset::maxp &&
            required_tables_bitset & required_tables_bitset::name &&
            required_tables_bitset & required_tables_bitset::post) {
            return true;
        }

        return false;
    }

    struct NameRecord {
        U16 platform_id;
        U16 platform_specific_id;
        U16 language_id;
        U16 name_id;
        U16 size; // Name string length in bytes.
        U16 offset; // Name string offset in bytes from stringOffset
    };

    static const constexpr Length name_record_size = sizeof(U16) * 6;

    struct NameTable {
        U16 format; // must be 0
        U16 count; // The number of NameRecord's in this name table.
        U16 string_offset; // Offset in bytes to the beginning of the name character strings.
        NameRecord* name_record_array; // array of NameRecord
        // pointer to this table's table directory
        TableDirectory* table_directory;
    };

    void free_name_table(NameTable* name_table) {
        if (name_table->name_record_array) {
            free(name_table->name_record_array);
            name_table->name_record_array = nullptr;
        }
    }

    static const constexpr Length name_table_size_without_name_record_array = sizeof(U16) * 3;

    [[nodiscard]] TableDirectory* find_first_table_directory_with_tag(FontDirectory* font_directory, TableDirectory_Tag tag) {
        for (Length i = 0; i < font_directory->offset_subtable.num_tables; ++i) {
            if (font_directory->table_directory_array[i].tag == static_cast<U32>(tag)) {
                return &font_directory->table_directory_array[i];
            }
        }

        return nullptr;
    }

    [[nodiscard]] bool read_name_table(platform_layer::FileIter* file_iter, FontDirectory* font_directory, NameTable* name_table) {
        TableDirectory* table_directory = find_first_table_directory_with_tag(font_directory, TableDirectory_Tag::NAME);
        if (!table_directory) {
            return false;
        }

        if (table_directory->size < name_table_size_without_name_record_array) {
            return false;
        }

        // table directories have already been validated that they are pointing to something in bounds
        TTE_ASSERT(platform_layer::has_enough_space_from_begin(file_iter, table_directory->offset, table_directory->size));
        
        file_iter->at = file_iter->begin + table_directory->offset;

        name_table->table_directory = table_directory;

        platform_layer::read_big_endian_and_move_unchecked(file_iter, &name_table->format);
        if (name_table->format != 0) {
            return false;
        }

        platform_layer::read_big_endian_and_move_unchecked(file_iter, &name_table->count);
        platform_layer::read_big_endian_and_move_unchecked(file_iter, &name_table->string_offset);
        const Length string_offset_full = table_directory->offset + name_table->string_offset;
        if (!platform_layer::has_enough_space_from_begin(file_iter, table_directory->offset + name_table->string_offset)) {
            return false;
        }

        if (!platform_layer::has_enough_space_from_at(file_iter, name_table->count * name_record_size)) {
            return false;
        }

        name_table->name_record_array = static_cast<NameRecord*>(malloc(sizeof(NameRecord) * name_table->count));
        memset(name_table->name_record_array, 0, sizeof(NameRecord) * name_table->count);
        for (Length i = 0; i < name_table->count; ++i) {
            NameRecord* record = &name_table->name_record_array[i];
            platform_layer::read_big_endian_and_move_unchecked(file_iter, &record->platform_id);
            platform_layer::read_big_endian_and_move_unchecked(file_iter, &record->platform_specific_id);
            platform_layer::read_big_endian_and_move_unchecked(file_iter, &record->language_id);
            platform_layer::read_big_endian_and_move_unchecked(file_iter, &record->name_id);
            platform_layer::read_big_endian_and_move_unchecked(file_iter, &record->size);
            platform_layer::read_big_endian_and_move_unchecked(file_iter, &record->offset);
            if (!platform_layer::has_enough_space_from_begin(file_iter, table_directory->offset + name_table->string_offset + record->offset, record->size)) {
                return false;
            }
        }

        return true;
    }

    struct CMAPSubtable {
        U16 format;
    };

    static const constexpr Length base_cmap_subtable_size = sizeof(U16);

    struct CMAPSubtableFormat4 : public CMAPSubtable {
        // format must be Set to 4
        // length in bytes
        U16 size;
        // only used for platform_id == 1 (Macintosh), other platforms this should be 0
        // Quickdraw language code + 1, or 0 if language independent
        U16 language_code;
        U16 seg_count_times_2;
        // 	2 * (2**FLOOR(log2(seg_count)))
        U16 search_range;
        // log2(search_range/2)
        U16 entry_selector;
        // 	(2 * seg_count) - search_range
        U16 range_shift;
        // Ending character code for each segment, last = 0xFFFF.
        // length is seg_count
        U16* end_code_array;
        // This value should be zero
        U16 reserved_pad;
        // Starting character code for each segment
        // length is seg_count
        U16* start_code_array;
        // Delta for all character codes in segment
        // length is seg_count
        U16* id_delta_array;
        // Offset in bytes to glyph_index_array, or 0
        // length is seg_count
        U16* id_range_offset_array;
        // 	Glyph index array
        // length is variable
        U16* glyph_index_array;
    };

    static const constexpr Length cmap_subtable_format_4_min_size_without_arrays = sizeof(U16) * 7;

    struct CMAPEncodingSubtable {
        U16 platform_id;
        U16 platform_specific_id;
        U32 offset;
        CMAPSubtable* subtable;
    };

    static const constexpr Length cmap_encoding_subtable_size = sizeof(U16) * 3;

    struct CMAPTable {
        // Version number (Set to zero)
        U16 version;
        U16 number_encoding_subtables;
        CMAPEncodingSubtable* encoding_subtable_array;
        TableDirectory* table_directory;
    };

    static const constexpr Length cmap_table_size_without_encoding_subtable_array = sizeof(U16) * 2;

    void free_cmap_table(CMAPTable* cmap_table) {
        if (cmap_table->encoding_subtable_array) {
            for (Length i = 0; i < cmap_table->number_encoding_subtables; ++i) {
                if (cmap_table->encoding_subtable_array[i].subtable) {
                    if (cmap_table->encoding_subtable_array[i].subtable->format == 4) {
                        CMAPSubtableFormat4* subtable = static_cast<CMAPSubtableFormat4*>(cmap_table->encoding_subtable_array[i].subtable);
                        if (subtable->end_code_array) {
                            free(subtable->end_code_array);
                            subtable->end_code_array = nullptr;
                        }
                        if (subtable->start_code_array) {
                            free(subtable->start_code_array);
                            subtable->start_code_array = nullptr;
                        }
                        if (subtable->id_delta_array) {
                            free(subtable->id_delta_array);
                            subtable->id_delta_array = nullptr;
                        }
                        if (subtable->id_range_offset_array) {
                            free(subtable->id_range_offset_array);
                            subtable->id_range_offset_array = nullptr;
                        }
                        if (subtable->glyph_index_array) {
                            free(subtable->glyph_index_array);
                            subtable->glyph_index_array = nullptr;
                        }
                    }
                    free(cmap_table->encoding_subtable_array[i].subtable);
                    cmap_table->encoding_subtable_array[i].subtable = nullptr;
                }
            }
            free(cmap_table->encoding_subtable_array);
            cmap_table->encoding_subtable_array = nullptr;
        }
    }

    [[nodiscard]] bool read_cmap_table(platform_layer::FileIter* file_iter, FontDirectory* font_directory, CMAPTable* cmap_table) {
        TableDirectory* table_directory = find_first_table_directory_with_tag(font_directory, TableDirectory_Tag::CMAP);
        if (!table_directory) {
            return false;
        }

        if (table_directory->size < cmap_table_size_without_encoding_subtable_array) {
            return false;
        }

        // table directories have already been validated that they are pointing to something in bounds
        TTE_ASSERT(platform_layer::has_enough_space_from_begin(file_iter, table_directory->offset, table_directory->size));
        
        file_iter->at = file_iter->begin + table_directory->offset;
        U8* const cmap_table_begin = file_iter->at;

        cmap_table->table_directory = table_directory;
        platform_layer::read_big_endian_and_move_unchecked(file_iter, &cmap_table->version);
        if (cmap_table->version != 0) {
            return false;
        }

        platform_layer::read_big_endian_and_move_unchecked(file_iter, &cmap_table->number_encoding_subtables);
        if (cmap_table->number_encoding_subtables == 0) {
            return false;
        }

        if (!platform_layer::has_enough_space_from_at(file_iter, cmap_table->number_encoding_subtables * cmap_encoding_subtable_size)) {
            return false;
        }

        cmap_table->encoding_subtable_array = static_cast<CMAPEncodingSubtable*>(malloc(sizeof(CMAPEncodingSubtable) * cmap_table->number_encoding_subtables));
        memset(cmap_table->encoding_subtable_array, 0, sizeof(CMAPEncodingSubtable) * cmap_table->number_encoding_subtables);
        for (Length i = 0; i < cmap_table->number_encoding_subtables; ++i) {
            platform_layer::read_big_endian_and_move_unchecked(file_iter, &cmap_table->encoding_subtable_array[i].platform_id);
            platform_layer::read_big_endian_and_move_unchecked(file_iter, &cmap_table->encoding_subtable_array[i].platform_specific_id);
            platform_layer::read_big_endian_and_move_unchecked(file_iter, &cmap_table->encoding_subtable_array[i].offset);

            if (!platform_layer::has_enough_space_from_begin(file_iter, cmap_table->encoding_subtable_array[i].offset, base_cmap_subtable_size)) {
                return false;
            }
            U8* const file_iter_at_copy = file_iter->at;
            file_iter->at = cmap_table_begin + cmap_table->encoding_subtable_array[i].offset;

            U16 format;
            platform_layer::read_big_endian_and_move_unchecked(file_iter, &format);

            if (format == 4) {
                if (!platform_layer::has_enough_space_from_at(file_iter, cmap_subtable_format_4_min_size_without_arrays - base_cmap_subtable_size)) {
                    return false;
                }

                CMAPSubtableFormat4* subtable;
                cmap_table->encoding_subtable_array[i].subtable = subtable = static_cast<CMAPSubtableFormat4*>(malloc(sizeof(CMAPSubtableFormat4)));
                memset(cmap_table->encoding_subtable_array[i].subtable, 0, sizeof(CMAPSubtableFormat4));
                subtable->format = format;
                platform_layer::read_big_endian_and_move_unchecked(file_iter, &subtable->size);
                if (!platform_layer::has_enough_space_from_at(file_iter, subtable->size - sizeof(subtable->size) - sizeof(format))) {
                    return false;
                }

                platform_layer::read_big_endian_and_move_unchecked(file_iter, &subtable->language_code);
                if (static_cast<NameRecord_And_CMAPEncodingSubtable_PlatformId>(cmap_table->encoding_subtable_array[i].platform_id) == NameRecord_And_CMAPEncodingSubtable_PlatformId::Macintosh) {
                    if (subtable->language_code < 0 || subtable->language_code > 33) {
                        return false;
                    }
                } else if (subtable->language_code != 0) {
                    return false;
                }

                platform_layer::read_big_endian_and_move_unchecked(file_iter, &subtable->seg_count_times_2);
                if (subtable->seg_count_times_2 % 2 != 0) {
                    return false;
                }

                const U16 seg_count = subtable->seg_count_times_2 / 2;
                platform_layer::read_big_endian_and_move_unchecked(file_iter, &subtable->search_range);
                platform_layer::read_big_endian_and_move_unchecked(file_iter, &subtable->entry_selector);
                platform_layer::read_big_endian_and_move_unchecked(file_iter, &subtable->range_shift);

                if (!platform_layer::has_enough_space_from_at(file_iter, (sizeof(U16) * seg_count * 4) + sizeof(U16))) {
                    return false;
                }

                subtable->end_code_array = static_cast<U16*>(malloc(sizeof(U16) * seg_count));
                memset(subtable->end_code_array, 0, sizeof(U16) * seg_count);
                for (U16 j = 0; j < seg_count; ++j) {
                    platform_layer::read_big_endian_and_move_unchecked(file_iter, &subtable->end_code_array[j]);
                }

                platform_layer::read_big_endian_and_move_unchecked(file_iter, &subtable->reserved_pad);
                if (subtable->reserved_pad != 0) {
                    return false;
                }

                subtable->start_code_array = static_cast<U16*>(malloc(sizeof(U16) * seg_count));
                memset(subtable->start_code_array, 0, sizeof(U16) * seg_count);
                for (U16 j = 0; j < seg_count; ++j) {
                    platform_layer::read_big_endian_and_move_unchecked(file_iter, &subtable->start_code_array[j]);
                }

                subtable->id_delta_array = static_cast<U16*>(malloc(sizeof(U16) * seg_count));
                memset(subtable->id_delta_array, 0, sizeof(U16) * seg_count);
                for (U16 j = 0; j < seg_count; ++j) {
                    platform_layer::read_big_endian_and_move_unchecked(file_iter, &subtable->id_delta_array[j]);
                }

                subtable->id_range_offset_array = static_cast<U16*>(malloc(sizeof(U16) * seg_count));
                memset(subtable->id_range_offset_array, 0, sizeof(U16) * seg_count);
                for (U16 j = 0; j < seg_count; ++j) {
                    platform_layer::read_big_endian_and_move_unchecked(file_iter, &subtable->id_range_offset_array[j]);
                }
            } else {
                cmap_table->encoding_subtable_array[i].subtable = static_cast<CMAPSubtable*>(malloc(sizeof(CMAPSubtable)));
                memset(cmap_table->encoding_subtable_array[i].subtable, 0, sizeof(CMAPSubtable));
                // NOTE(TB): not supporting this format yet, mark as invalid
                cmap_table->encoding_subtable_array[i].subtable->format = 0;
            }

            file_iter->at = file_iter_at_copy;
        }
        return true;
    }

    static void print(FontDirectory* font_directory) {
        TTE_DBG("tag\tlength\toffset\n");
        for (Length i = 0; i < font_directory->offset_subtable.num_tables; ++i) {
            TTE_DBG("%c%c%c%c\t%d\t%d\n", reinterpret_cast<U8*>(&font_directory->table_directory_array[i].tag)[3], reinterpret_cast<U8*>(&font_directory->table_directory_array[i].tag)[2], reinterpret_cast<U8*>(&font_directory->table_directory_array[i].tag)[1], reinterpret_cast<U8*>(&font_directory->table_directory_array[i].tag)[0], font_directory->table_directory_array[i].size, font_directory->table_directory_array[i].offset);
        }
    }

    static void print_english(platform_layer::FileIter* file_iter, NameTable* name_table) {
        TTE_DBG("==== Name Table\n");
        TTE_DBG("\tformat\tcount\tstring offset\n");
        TTE_DBG("\t%d\t%d\t%d\n", name_table->format, name_table->count, name_table->string_offset);
        TTE_DBG("\t==== Name Record\n");
        for (Length i = 0; i < name_table->count; ++i) {
            NameRecord& r = name_table->name_record_array[i];
            
            if (r.platform_id == 0 || (r.platform_id == 1 && r.language_id == 0) || (r.platform_id == 3 && r.language_id == 1033)) {
                char* name = static_cast<char*>(malloc(sizeof(char) * r.size + 1));
                memcpy(name, file_iter->begin + name_table->table_directory->offset + name_table->string_offset + r.offset, r.size);
                name[r.size] = '\0';
                TTE_DBG("\t\t%s\n", name);
                TTE_DBG("\t\t\tplatform_id\tplatform_specific_id\tlanguage_id\n");
                TTE_DBG("\t\t\t%d\t%d\t%d\n", r.platform_id, r.platform_specific_id, r.language_id);
                free(name);
            }
        }
    }

    static void print(CMAPTable* cmap_table) {
        TTE_DBG("==== CMAP Table");
        TTE_DBG("\tversion\tnumber_subtables");
        TTE_DBG("\t%d\t%d", cmap_table->version, cmap_table->number_encoding_subtables);
        for (Length i = 0; i < cmap_table->number_encoding_subtables; ++i) {
            TTE_DBG("\t\t ==== Subtable");
            TTE_DBG("\t\t\tplatform_id: %d", cmap_table->encoding_subtable_array[i].platform_id);
            TTE_DBG("\t\t\tplatform_specific_id: %d", cmap_table->encoding_subtable_array[i].platform_specific_id);
            TTE_DBG("\t\t\toffset: %d", cmap_table->encoding_subtable_array[i].offset);
            TTE_DBG("\t\t\tformat: %d", cmap_table->encoding_subtable_array[i].subtable->format);
            if (cmap_table->encoding_subtable_array[i].subtable->format == 4) {
                CMAPSubtableFormat4* subtable = static_cast<CMAPSubtableFormat4*>(cmap_table->encoding_subtable_array[i].subtable);
                TTE_DBG("\t\t\tsize: %d", subtable->size);
                TTE_DBG("\t\t\tlanguage_code: %d", subtable->language_code);
                TTE_DBG("\t\t\tseg_count_times_2: %d", subtable->seg_count_times_2);
                TTE_DBG("\t\t\tsearch_range: %d", subtable->search_range);
                TTE_DBG("\t\t\tentry_selector: %d", subtable->entry_selector);
                TTE_DBG("\t\t\trange_shift: %d", subtable->range_shift);
            }
        }
    }

    void parse_file(const char* file_path) {
        Length file_size;
        platform_layer::FileIter file_iter;
        file_iter.begin = file_iter.at = platform_layer::read_file(file_path, &file_iter.size);
        if (!file_iter.begin) {
            TTE_DBG("failed to read file\n");
            return;
        }

        FontDirectory font_directory{};
        if (read_font_directory(&file_iter, &font_directory)) {
            print(&font_directory);
            NameTable name_table{};
            if (read_name_table(&file_iter, &font_directory, &name_table)) {
                print_english(&file_iter, &name_table);
            }
            free_name_table(&name_table);
            CMAPTable cmap_table{};
            if (read_cmap_table(&file_iter, &font_directory, &cmap_table)) {
                print(&cmap_table);
            }
            free_cmap_table(&cmap_table);
        }
        free_font_directory(&font_directory);
    }
}}
