using System.Text;
using System.IO;

namespace FileArchiveToolTest
{
	class FileHeader
	{
		protected byte m_pathLen;					// 目录长度，不包括 '\0'
		protected string m_pFullPath;
		protected string m_fileNamePath;		// 文件路径名字
		protected uint m_fileOffset;				// 文件在整个 Archive 中的偏移
		protected uint m_fileSize;					// 文件大小

		public FileHeader()
		{
			
		}

		public void readHeaderFromArchiveFile(MByteBuffer ba)
		{
			m_pathLen = ba.readUnsignedByte();
			m_fileNamePath = ba.readMultiByte(m_pathLen, Encoding.UTF8);
			m_fileOffset = ba.readUnsignedInt();
			m_fileSize = ba.readUnsignedInt();
		}

		public void writeArchiveFile2File(FileStream fileHandle, UnArchiveParam pUnArchiveParam)
		{
			m_pFullPath = pUnArchiveParam.getUnArchiveOutDir() + "/" + m_fileNamePath;

			fileHandle.Seek(m_fileOffset, SeekOrigin.Begin);	// 移动到文件开始位置

			FileStream localFile = new FileStream(m_pFullPath, FileMode.Create);
			byte[] pchar;

			if (localFile != null)
			{
				// 一次性读取进来，可能需要压缩
				pchar = new byte[m_fileSize + 1];

				uint readlength = (uint)fileHandle.Read(pchar, 0, (int)m_fileSize);
				if (readlength == m_fileSize)
				{
					if (false)		// 如果不压缩
					{
						localFile.Write(pchar, 0, (int)m_fileSize);
					}
					else	// 需要解压
					{
						byte[] retChar = null;
						m_fileSize = MLzma.DecompressStrLZMA(pchar, retChar);
						localFile.Write(pchar, 0, (int)m_fileSize);
					}
				}
				else							// 读取可能有问题，读取不了完整文件
				{

				}

				fileHandle.Close();
			}
		}
	}
}