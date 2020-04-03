#include "parser.h"


namespace parser::statement::impl
{
    typedef CloneableAllocator<CodeFragment> CodeFragmentAlloc;
    typedef CloneableAllocator<Statement> StatementAlloc;
    typedef CodeFragmentList::Pointer Ptr;

    static StatementAlloc packPart(Ptr& it);
    static StatementAlloc packPreUnary(Ptr& it);
    static StatementAlloc packPostUnary(Ptr& it, const Statement& pre);
    static const Operator* findNextOperatorSymbol(const CodeFragmentList& list, size_t index);
    static StatementAlloc getSuperOperatorScope(Ptr& it, const Operator& base);
    static StatementAlloc packOperation(Ptr& it, const Statement& operand1);
    static StatementAlloc packNextOperatorPart(Ptr& it, const Operator& oper);
}


namespace parser::statement
{
	CloneableAllocator<Statement> parse(const CodeFragmentList& list)
	{
		impl::Ptr it = list.ptr();
        impl::StatementAlloc operand = impl::packPart(it);
		if (!it)
			return operand;
		return impl::packOperation(it, operand);
	}
}





namespace parser::statement::impl
{
	static ParserError error(const size_t line, const std::string& msg = "") { return { line, msg.c_str() }; }
	static ParserError error(const Ptr& it, const std::string& msg = "") { return { it.line(), msg.c_str() }; }
	static ParserError error(const CodeFragmentList& list, const std::string& msg = "") { return { list.sourceLine(), msg.c_str() }; }

	StatementAlloc packPart(Ptr& it)
	{
		if (!it)
			throw error(it, "Unexpected end of instruction");
		return packPostUnary(it, packPreUnary(it));
	}

	StatementAlloc packPreUnary(Ptr& it)
	{
		CodeFragmentAlloc part = *it;
        it++;
        if (part->is(CodeFragmentType::Operator))
        {
            if (!it)
                throw error(it, "unexpected end of instruction");

            const Operator& prefix = part->as<Operator>();
            if (!prefix.isUnary())
                throw error(it, "Operator " + prefix.toString() + " cannot be a non unary prefix operator");

            part = packNextOperatorPart(it, part->as<Operator>());
            if (!part->isStatement())
                throw error(it, "Expected valid operand. But found: " + part->toString());

            return Operation::unary(prefix, part->as<Statement>());
        }
        if (!part->isStatement())
            throw error(it, "Expected valid operand. But found: " + part->toString());
        return part->as<Statement>();
	}

	StatementAlloc packPostUnary(Ptr& it, const Statement& pre)
	{
        if (!it)
            return pre;
        CodeFragmentAlloc part = *it;

        if (part->is(CodeFragmentType::Operator))
        {
            const Operator& sufix = part->as<Operator>();
            if (!sufix.isUnary())
                return pre;

            it++;
            if (sufix.hasRightToLeft())
                throw error(it, "Operator " + sufix.toString() + " cannot be an unary sufix operator");

            return packPostUnary(it, Operation::unary(sufix, pre));
        }
        return pre;
	}

    const Operator* findNextOperatorSymbol(const CodeFragmentList& list, size_t index)
    {
        size_t len = list.size();
        for (size_t i = index; i < len; i++)
            if (list[i].is(CodeFragmentType::Operator))
                return &list[i].as<Operator>();
        return nullptr;
    }

    StatementAlloc getSuperOperatorScope(Ptr& it, const Operator& base)
    {
        size_t start = it.index();
        for (; it; it++)
        {
            if (!it->is(CodeFragmentType::Operator))
                continue;

            const Operator& op = it->as<Operator>();
            if (base.comparePriority(op) > 0)
            {
                //it.decrease();
                return parse(it.list().sublist(start, it.index() - start));
            }
        }
        return parse(it.list().sublist(start));
    }

    StatementAlloc packOperation(Ptr& it, const Statement& operand1)
    {
        if (!it->is(CodeFragmentType::Operator))
            throw error(it, "Expected a valid operator between operands. \"" + it->toString() + "\"");
        const Operator& oper = it->as<Operator>();
        it++;
        StatementAlloc operation;

        if (oper.isTernary())
        {
            size_t start = it.index();
            size_t terOp = 0;
            for (; it; it++)
            {
                const CodeFragment& c = *it;
                if (c == Operator::TernaryConditional)
                    terOp++;
                else if (c == Stopchar::TwoPoints)
                {
                    if (terOp == 0)
                        break;
                    terOp--;
                }
            }
            if (!it)
                throw error(it, "Expected a : in ternary operator");

            const Statement& response1 = parse(it.list().sublist(start, it.index() - start));
            it++;
            const Statement& response2 = parse(it.list().sublist(it.index()));
            it.finish();
            return Operation::ternary(operand1, response1, response2);
        }
        else if (oper.isBinary())
        {
            const Statement& operand2 = packNextOperatorPart(it, oper);
            operation = Operation::binary(oper, operand1, operand2);
        }
        else if (oper.isAssignment())
        {
            const Statement& operand2 = packNextOperatorPart(it, oper);
            operation = Operation::assignment(oper, operand1, operand2);
        }
        /*else if (operator.isCall())
        {
            if (it.end())
                throw new CompilerError("Expected a valid arguments list in call operator.");
            CodeFragment args = it.listValue();
            it.increase();
            operation = Operation.call(operand1, args);
        }*/
        else throw error(it, "Invalid operator type: " + oper.toString());


        if (!it)
            return operation;
        return packOperation(it, operation);
    }

    StatementAlloc packNextOperatorPart(Ptr& it, const Operator& oper)
    {
        const Operator* nextOperator = findNextOperatorSymbol(it.list(), it.index());
        if (nextOperator && oper.comparePriority(*nextOperator) >= 0)
            nextOperator = nullptr;

        StatementAlloc operand2;
        if (nextOperator)
            operand2 = getSuperOperatorScope(it, oper);
        else operand2 = packPart(it);

        return operand2;
    }
}
