#include "../includes/MimeTypes.hpp"

MimeTypes::MimeTypes() {
	this->mime_types["html"] = "text/html";
	this->mime_types["htm"] = "text/html";
	this->mime_types["shtml"] = "text/html";
	this->mime_types["css"] = "text/css";
	this->mime_types["xml"] = "text/xml";
	this->mime_types["gif"] = "image/gif";
	this->mime_types["jpeg"] = "image/jpeg";
	this->mime_types["jpg"] = "image/jpeg";
	this->mime_types["js"] = "application/javascript";
	this->mime_types["atom"] = "application/atom+xml";
	this->mime_types["rss"] = "application/rss+xml";
	this->mime_types["mml"] = "text/mathml";
	this->mime_types["txt"] = "text/plain";
	this->mime_types["jad"] = "text/vnd.sun.j2me.app-descriptor";
	this->mime_types["wml"] = "text/vnd.wap.wml";
	this->mime_types["htc"] = "text/x-component";
	this->mime_types["avif"] = "image/avif";
	this->mime_types["png"] = "image/png";
	this->mime_types["svg"] = "image/svg+xml";
	this->mime_types["svgz"] = "image/svg+xml";
	this->mime_types["tif"] = "image/tiff";
	this->mime_types["tiff"] = "image/tiff";
	this->mime_types["wbmp"] = "image/vnd.wap.wbmp";
	this->mime_types["webp"] = "image/webp";
	this->mime_types["ico"] = "image/x-icon";
	this->mime_types["jng"] = "image/x-jng";
	this->mime_types["bmp"] = "image/x-ms-bmp";
	this->mime_types["woff"] = "font/woff";
	this->mime_types["woff2"] = "font/woff2";
	this->mime_types["jar"] = "application/java-archive";
	this->mime_types["war"] = "application/java-archive";
	this->mime_types["ear"] = "application/java-archive";
	this->mime_types["json"] = "application/json";
	this->mime_types["hqx"] = "application/mac-binhex40";
	this->mime_types["doc"] = "application/msword";
	this->mime_types["pdf"] = "application/pdf";
	this->mime_types["ps"] = "application/postscript";
	this->mime_types["eps"] = "application/postscript";
	this->mime_types["ai"] = "application/postscript";
	this->mime_types["rtf"] = "application/rtf";
	this->mime_types["m3u8"] = "application/vnd.apple.mpegurl";
	this->mime_types["kml"] = "application/vnd.google-earth.kml+xml";
	this->mime_types["kmz"] = "application/vnd.google-earth.kmz";
	this->mime_types["xls"] = "application/vnd.ms-excel";
	this->mime_types["eot"] = "application/vnd.ms-fontobject";
	this->mime_types["ppt"] = "application/vnd.ms-powerpoint";
	this->mime_types["odg"] = "application/vnd.oasis.opendocument.graphics";
	this->mime_types["odp"] = "application/vnd.oasis.opendocument.presentation";
	this->mime_types["ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	this->mime_types["odt"] = "application/vnd.oasis.opendocument.text";
	this->mime_types["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	this->mime_types["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	this->mime_types["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	this->mime_types["wmlc"] = "application/vnd.wap.wmlc";
	this->mime_types["wasm"] = "application/wasm";
	this->mime_types["7z"] = "application/x-7z-compressed";
	this->mime_types["cco"] = "application/x-cocoa";
	this->mime_types["jardiff"] = "application/x-java-archive-diff";
	this->mime_types["jnlp"] = "application/x-java-jnlp-file";
	this->mime_types["run"] = "application/x-makeself";
	this->mime_types["pl"] = "application/x-perl";
	this->mime_types["pm"] = "application/x-perl";
	this->mime_types["prc"] = "application/x-pilot";
	this->mime_types["pdb"] = "application/x-pilot";
	this->mime_types["rar"] = "application/x-rar-compressed";
	this->mime_types["rpm"] = "application/x-redhat-package-manager";
	this->mime_types["sea"] = "application/x-sea";
	this->mime_types["swf"] = "application/x-shockwave-flash";
	this->mime_types["sit"] = "application/x-stuffit";
	this->mime_types["tcl"] = "application/x-tcl";
	this->mime_types["tk"] = "application/x-tcl";
	this->mime_types["der"] = "application/x-x509-ca-cert";
	this->mime_types["pem"] = "application/x-x509-ca-cert";
	this->mime_types["crt"] = "application/x-x509-ca-cert";
	this->mime_types["xpi"] = "application/x-xpinstall";
	this->mime_types["xhtml"] = "application/xhtml+xml";
	this->mime_types["xspf"] = "application/xspf+xml";
	this->mime_types["zip"] = "application/zip";
	this->mime_types["bin"] = "application/octet-stream";
	this->mime_types["exe"] = "application/octet-stream";
	this->mime_types["dll"] = "application/octet-stream";
	this->mime_types["deb"] = "application/octet-stream";
	this->mime_types["dmg"] = "application/octet-stream";
	this->mime_types["iso"] = "application/octet-stream";
	this->mime_types["img"] = "application/octet-stream";
	this->mime_types["msi"] = "application/octet-stream";
	this->mime_types["msp"] = "application/octet-stream";
	this->mime_types["msm"] = "application/octet-stream";
	this->mime_types["mid"] = "audio/midi";
	this->mime_types["midi"] = "audio/midi";
	this->mime_types["kar"] = "audio/midi";
	this->mime_types["mp3"] = "audio/mpeg";
	this->mime_types["ogg"] = "audio/ogg";
	this->mime_types["m4a"] = "audio/x-m4a";
	this->mime_types["ra"] = "audio/x-realaudio";
	this->mime_types["3gpp"] = "video/3gpp";
	this->mime_types["3gp"] = "video/3gpp";
	this->mime_types["ts"] = "video/mp2t";
	this->mime_types["mp4"] = "video/mp4";
	this->mime_types["mpeg"] = "video/mpeg";
	this->mime_types["mpg"] = "video/mpeg";
	this->mime_types["mov"] = "video/quicktime";
	this->mime_types["webm"] = "video/webm";
	this->mime_types["flv"] = "video/x-flv";
	this->mime_types["m4v"] = "video/x-m4v";
	this->mime_types["mng"] = "video/x-mng";
	this->mime_types["asx"] = "video/x-ms-asf";
	this->mime_types["asf"] = "video/x-ms-asf";
	this->mime_types["wmv"] = "video/x-ms-wmv";
	this->mime_types["avi"] = "video/x-msvideo";

	// Executables
	
	this->mime_types["bin"] = "application/octet-stream";
	this->mime_types["elf"] = "application/x-executable";
	this->mime_types["jar"] = "application/java-archive";
	this->mime_types["pl"] = "application/x-perl";
	this->mime_types["py"] = "text/x-python";
	this->mime_types["rb"] = "application/x-ruby";
	this->mime_types["sh"] = "application/x-sh";
	this->mime_types["js"] = "application/javascript";
	this->mime_types["bat"] = "application/bat";
	this->mime_types["ps1"] = "application/x-powershell";
	this->mime_types["avi"] = "video/x-msvideo";
	this->mime_types["php"] = "application/x-httpd-php";
	this->mime_types["c"] = "text/x-c";
	this->mime_types["cpp"] = "text/x-c++";
	this->mime_types["java"] = "text/x-java-source";
	this->mime_types["cs"] = "text/x-csharp";
	this->mime_types["swift"] = "text/x-swift";
	this->mime_types["go"] = "text/x-go";
	this->mime_types["rust"] = "text/rust";
	this->mime_types["dart"] = "application/dart";
	this->mime_types["kotlin"] = "text/x-kotlin";
}

MimeTypes::~MimeTypes() {}

MimeTypes::MimeTypes(const MimeTypes &other) {
	*this = other;
}

MimeTypes &MimeTypes::operator=(const MimeTypes &other) {
	if (this != &other) {
		this->mime_types = other.mime_types;
	}
	return (*this);
}

// GETTERS
std::string MimeTypes::getMimeType(std::string filename)
{
	std::string extension;
	size_t dot_pos;

	dot_pos = filename.find_last_of(".");
	if (dot_pos != std::string::npos)
		extension = filename.substr(dot_pos + 1);
	else
		return ("application/octet-stream");
	return (this->mime_types[extension]);
}

// PUBLIC

bool MimeTypes::isMimeTypeValid(std::string filename)
{
	std::string extension;
	size_t dot_pos;

	dot_pos = filename.find_last_of(".");
	if (dot_pos != std::string::npos)
		extension = filename.substr(dot_pos + 1);
	else
		return (false);
	if (
			extension != "c" 
			&& extension != "cpp" 
			&& extension != "java" 
			&& extension != "cs" 
			&& extension != "swift" 
			&& extension != "go" 
			&& extension != "rust" 
			&& extension != "dart" 
			&& extension != "kotlin" 
			&& extension != "py" 
			&& extension != "rb" 
			&& extension != "sh" 
			&& extension != "bat" 
			&& extension != "ps1" 
			&& extension != "php" 
			&& extension != "pl" 
			&& extension != "bin" 
			&& extension != "elf" 
			&& extension != "jar" 
			&& extension != "avi"
			&& extension != "cgi")
		return (true);
	return (false);
}
