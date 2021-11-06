set_xmakever("2.5.1")

set_project("picross")
set_languages("cxx20")

-- modes
add_rules("mode.debug", "mode.release")

-- packages
add_requires("fmt 8")
add_requires("tl_expected 1.0.0")

set_warnings("allextra")

warnings = {
    "-Wconversion",
    "-Walloca",
    "-Wcast-qual",
    "-Wformat-security",
    "-Wnull-dereference",
    "-Warray-bounds",
    "-Warray-bounds-pointer-arithmetic",
    "-Wassign-enum",
    "-Wbad-function-cast",
    "-Wconditional-uninitialized",
    "-Wfloat-equal",
    "-Wformat",
    "-Wformat-type-confusion",
    "-Wimplicit-fallthrough",
    "-Widiomatic-parentheses",
    "-Wloop-analysis",
    "-Wpointer-arith",
    "-Wshift-sign-overflow",
    "-Wshorten-64-to-32",
    "-Wswitch-enum",
    "-Wtautological-constant-in-range-compare",
    "-Wunreachable-code-aggressive",
    "-Wthread-safety",
    "-Wcomma"
}

for _, w in pairs(warnings) do
    add_cxflags(w)
end

if is_mode("debug") then
    add_defines("DEBUG")

    set_symbols("debug")

    set_optimize("fastest")

    add_cxflags("-fsanitize=address,undefined,integer -g -fno-omit-frame-pointer")
    add_ldflags("-fsanitize=address,undefined,integer -g -fno-omit-frame-pointer")
end

if is_mode("release") then
    add_cxflags("-fomit-frame-pointer")
    add_defines("NDEBUG")
    set_optimize("fastest")
end

-- binary
target("picross")
    set_kind("binary")

    add_headerfiles("src/*.h")
    add_files("src/*.cpp")

    add_packages("fmt")
    add_packages("tl_expected")
