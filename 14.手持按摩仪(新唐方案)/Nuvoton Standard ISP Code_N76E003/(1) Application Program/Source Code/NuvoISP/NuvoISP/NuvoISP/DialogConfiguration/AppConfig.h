// AppConfig.h: interface for the CAppConfig class.
//
//////////////////////////////////////////////////////////////////////

#ifndef INC__APP_CONFIG_H__
#define INC__APP_CONFIG_H__


// class CAppConfig_6543
	// : public CConfig
class CAppConfig
{
public:
	virtual ~CAppConfig();

	/* Global configurations */

	/* NUC1xx configurations */
	struct NUC1xx_configs_t
	{
		unsigned int m_value[2];

		// NUC1xx_configs_t(){}
		// NUC1xx_configs_t(const CAppConfig_6516::NUC1xx_configs_t &right)
		// {
			// memcpy(m_value, right.m_value, sizeof(m_value));
		// }
	};

	/* NUC4xx configurations */
	struct NUC4xx_configs_t
	{
		//unsigned int m_value[2];
		unsigned int m_value[4];		//CY 2013/6/7

		// NUC4xx_configs_t(){}
		// NUC4xx_configs_t(const CAppConfig_6516::NUC4xx_configs_t &right)
		// {
			// memcpy(m_value, right.m_value, sizeof(m_value));
		// }
	};

	/* M451 configurations */
	struct M451_configs_t
	{
		unsigned int m_value[2];
		//unsigned int m_value[4];

		// M451_configs_t(){}
		// M451_configs_t(const CAppConfig_6516::M451_configs_t &right)
		// {
			// memcpy(m_value, right.m_value, sizeof(m_value));
		// }
	};

	/* M05x configurations */
	struct M05x_configs_t
	{
		unsigned int m_value[2];

		// M05x_configs_t(){}
		// M05x_configs_t(const CAppConfig_6516::M05x_configs_t &right)
		// {
			// memcpy(m_value, right.m_value, sizeof(m_value));
		// }
	};

	/* M0564 configurations */
	struct M0564_configs_t
	{
		unsigned int m_value[2];
	};

	/* Nano100 configurations */
	struct Nano100_configs_t
	{
		unsigned int m_value[2];

		// Nano100_configs_t(){}
		// Nano100_configs_t(const CAppConfig_6516::Nano100_configs_t &right)
		// {
			// memcpy(m_value, right.m_value, sizeof(m_value));
		// }
	};

	/* Mini51 configurations */
	struct Mini51_configs_t
	{
		unsigned int m_value[2];

		// Mini51_configs_t(){}
		// Mini51_configs_t(const CAppConfig_6516::Mini51_configs_t &right)
		// {
			// memcpy(m_value, right.m_value, sizeof(m_value));
		// }
	};

	/* N572 configurations */
	struct N572_configs_t
	{
		unsigned int m_value[5];

		// N572_configs_t(){}
		// N572_configs_t(const CAppConfig_6516::N572_configs_t &right)
		// {
			// memcpy(m_value, right.m_value, sizeof(m_value));
		// }
	};

	/* MT5xx configurations */
	struct MT5xx_configs_t
	{
		unsigned int m_value[2];

		// MT5xx_configs_t(){}
		// MT5xx_configs_t(const CAppConfig_6516::MT5xx_configs_t &right)
		// {
			// memcpy(m_value, right.m_value, sizeof(m_value));
		// }
	};

	/* AU91xx configurations */
	struct AU91xx_configs_t
	{
		unsigned int m_value[2];

		// AU91xx_configs_t(){}
		// AU91xx_configs_t(const CAppConfig_6516::AU91xx_configs_t &right)
		// {
			// memcpy(m_value, right.m_value, sizeof(m_value));
		// }
	};

	/* NUC505 configurations */

	/* NUC029 configurations */
	struct NUC029_configs_t
	{
		unsigned int m_value[2];

		// NUC029_configs_t(){}
		// NUC029_configs_t(const CAppConfig_6516::NUC029_configs_t &right)
		// {
			// memcpy(m_value, right.m_value, sizeof(m_value));
		// }
	};

	/* NM1120 configurations */
	struct NM1120_configs_t
	{
		unsigned int m_value[2];

		// NM1120_configs_t(){}
		// NM1120_configs_t(const CAppConfig_6516::NM1120_configs_t &right)
		// {
			// memcpy(m_value, right.m_value, sizeof(m_value));
		// }
	};

	/* 8051 1T configurations */
	struct N76E1T_configs_t
	{
		unsigned int m_value[2];
	};

	struct TC8226_configs_t
	{
		//unsigned int m_value[2];
		unsigned int m_value[4];
	};
};

#endif // !defined(AFX_APPCONFIG_H__B96EEC5C_5D1E_4040_9CFB_1A234396F753__INCLUDED_)
