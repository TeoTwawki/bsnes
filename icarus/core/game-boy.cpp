auto Icarus::gameBoyManifest(const string& location) -> string {
  vector<uint8_t> buffer;
  concatenate(buffer, {location, "program.rom"});
  return gameBoyManifest(buffer, location);
}

auto Icarus::gameBoyManifest(vector<uint8_t>& buffer, const string& location) -> string {
  GameBoyCartridge cartridge{buffer.data(), buffer.size()};
  if(auto markup = cartridge.markup) {
    markup.append("\n");
    markup.append("information\n");
    markup.append("  sha256: ", Hash::SHA256(buffer.data(), buffer.size()).digest(), "\n");
    markup.append("  title:  ", prefixname(location), "\n");
    markup.append("  note:   ", "heuristically generated by icarus\n");
    return markup;
  }
  return "";
}

auto Icarus::gameBoyImport(vector<uint8_t>& buffer, const string& location) -> bool {
  auto name = prefixname(location);
  auto source = pathname(location);
  string target{settings["Library/Location"].text(), "Game Boy/", name, ".gb/"};
//if(directory::exists(target)) return failure("game already exists");

  string markup;

  if(settings["icarus/UseHeuristics"].boolean() && !markup) {
    GameBoyCartridge cartridge{buffer.data(), buffer.size()};
    if(markup = cartridge.markup) {
      markup.append("\n");
      markup.append("information\n");
      markup.append("  title: ", name, "\n");
      markup.append("  note:  heuristically generated by icarus\n");
    }
  }

  if(!markup) return failure("failed to parse ROM image");
  if(!directory::create(target)) return failure("library path unwritable");

  if(settings["icarus/CreateManifests"].boolean()) file::write({target, "manifest.bml"}, markup);
  file::write({target, "program.rom"}, buffer);
  return success();
}
