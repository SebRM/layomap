/*
 *    Copyright 2022 Matthias Kühlewein
 *    Copyright 2022 Kai Pastor
 *
 *    This file is part of OpenOrienteering.
 *
 *    OpenOrienteering is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    OpenOrienteering is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with OpenOrienteering.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OPENORIENTEERING_OCD_PARAMETER_STREAM_READER_H
#define OPENORIENTEERING_OCD_PARAMETER_STREAM_READER_H

#include <QString>
#include <QStringRef>

namespace OpenOrienteering {


/**
 * A class for processing OCD parameter strings,
 * loosely modeled after QXmlStreamReader.
 */
class OcdParameterStreamReader
{
public:
	/**
	 * Constructs a new reader object.
	 */
	explicit OcdParameterStreamReader(const QString& param_string) noexcept;
	
	/**
	 * Reads the input until the next key-value pair.
	 * 
	 * @return True if another key-value pair was reached. False for end of input.
	 */
	bool readNext();
	
	/**
	 * Returns the current key.
	 */
	char key() const { return current_key; }
	
	/**
	 * Returns the current value.
	 */
	QStringRef value() const;
	
	/**
	 * Returns true if there is no more data.
	 */
	bool atEnd() const { return next == -1; }
	
	/**
	 * The value returned by `key()` for the first value, or at the end of input.
	 */
	static constexpr char noKey() { return 0; }
	
private:
	QString param_string;
	int pos;  ///< The begin of the current value substring, or param_string.length().
	int next; ///< The position after the end of the current value substring, or -1 if reaching the end of input.
	char current_key;
};

}  // namespace Openorienteering

#endif // OPENORIENTEERING_OCD_PARAMETER_STREAM_READER
