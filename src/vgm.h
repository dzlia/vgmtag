#include <afc/stream.h>
#include <afc/Exception.h>
#include <cstdint>
#include <string>
#include <cstddef>

namespace vgm
{
	class VGMFile
	{
	public:
		enum class Format
		{
			vgm, vgz
		};

		VGMFile(const char * const srcFile);
		~VGMFile() {delete[] m_data;}

		void save(const char * const dest, const Format format);

		/*
		 * a) all tag values must be consecutive integers starting from 0
		 * b) title must be the first tag value
		 * c) notes must be the last tag value
		 */
		enum class Tag
		{
			title = 0, titleJP = 1, game = 2, gameJP = 3, system = 4, systemJP = 5,
			author = 6, authorJP = 7, date = 8, converter = 9, notes = 10
		};

		void setTag(const Tag name, const std::u16string &value);
		const std::u16string &getTag(const Tag name) const;

		Format getFormat() const {return m_format;}
	private:
		VGMFile(const VGMFile &) = delete;
		VGMFile &operator=(const VGMFile &) = delete;

		void normalise();

		void readHeader(afc::InputStream &in, size_t &cursor);
		void readGD3Info(afc::InputStream &in, size_t &cursor);
		void readData(afc::InputStream &in, size_t &cursor);

		void writeContent(afc::OutputStream &out) const;

		struct VGMHeader
		{
			// absolute positions of VGM header constituents in the header
			static const uint32_t POS_EOF = 0x4, POS_SN_CLOCK = 0xc, POS_YM2413_CLOCK = 0x10, POS_GD3 = 0x14, POS_LOOP = 0x1c,
						POS_YM2112_CLOCK = 0x2c, POS_YM2151_CLOCK = 0x30, POS_VGM_DATA = 0x34;

			/* Despite of the platform endianness these values are stored in files in the little-endian format and
			   are converted into the platform format while parsing the file. */
			static const uint32_t VGM_FILE_ID = 0x206d6756; // 'Vgm ' as 4 bytes casted to little-endian int32
			static const uint32_t VGM_FILE_VERSION = 0x00000150;

			static const uint32_t HEADER_SIZE = 0x40;

			uint32_t id, eofOffset, version, snClock,
					ym2413Clock, gd3Offset, sampleCount, loopOffset,
					loopSampleCount, rate, snFeedback, ym2612Clock,
					ym2151Clock, vgmDataOffset;
		};
		struct GD3Info
		{
			/* Despite of the platform endianness these values are stored in files in the little-endian format and
			   are converted into the platform format while parsing the file. */
			static const uint32_t VGM_FILE_GD3_ID = 0x20336447; // 'Gd3 ' as 4 bytes casted to little-endian int32
			static const uint32_t VGM_FILE_GD3_VERSION = 0x00000100;

			static const uint32_t HEADER_SIZE = 0x0c;

			size_t dataSize;
			std::u16string tags[static_cast<size_t>(Tag::notes) + 1];
		};

		VGMHeader m_header;
		GD3Info m_gd3Info;
		unsigned char *m_data;
		size_t m_dataSize;
		Format m_format;
	};
}
